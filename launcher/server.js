'use strict';

/**********************************************************************/
// COMMON SERVER
/**********************************************************************/

process.env.PORT = 8888

/**********************************************************************/
// GLOBAL PROGRAM VERSION
/**********************************************************************/

global.version = process.env.VERSION || 'unknown';


/**********************************************************************/
// THE REAL PROGRAM THAT WILL BE REPLICATED
/**********************************************************************/

const express = require('express');
const path = require('path');

/**********************************************************************/
// CONSTANTS
/**********************************************************************/

const PORT = process.env.PORT;
const HOST = process.env.HOST || '0.0.0.0';
if( !PORT || !HOST ){
    throw 'No PORT or HOST defined';
}

/**********************************************************************/
// APP
/**********************************************************************/

let app = express();

/**********************************************************************/
// CORS
/**********************************************************************/

const cors = require('cors');
app.use( cors() );

/**********************************************************************/
// BODY PARSER
/**********************************************************************/

const bodyParser = require('body-parser');
app.use( bodyParser.json() ); // NEEDED FOR LOGIN PARSING
app.use( bodyParser.urlencoded({ extended : true }) ); // EXTENDED TRUE NEEDE FOR PARSING PAYPAL IPN MESSAGES

/**********************************************************************/
// API ROUTES, will have cookie
/**********************************************************************/

app.get('/', (req, res) => {
    res.redirect( 'https://geoworldsim.com' );
});

app.post('/' , async (req, res) => {
    
    try {
        
        let configuration = req.body;
    
        if( !configuration.target || !configuration.id ){
            return res.status(404).send();
        }

        const fetch = require('node-fetch');
        const { spawn } = require('child_process');
        const fs = require('fs');
        let filename = `${__dirname}/${configuration.id}.json`;
        
        // CREATE CONFIG FILE
        fs.writeFileSync( filename , JSON.stringify(configuration) , 'utf8');
        
        // SPAWN CHILDREN PASSING CONFIG FILE
        var child = spawn( `${__dirname}/targets/${configuration.target}`, [ `config_file=${filename}` ] );
        
        // SET TIMEOUT
        let timer = setTimeout( () => { child.kill() } , (timeout * 1000) );
        child.stdout.on('data', (data) => {
            console.log(data.toString());
        });
        child.stderr.on('data', (data) => {
            console.log(data.toString());
        });
        child.on('exit', (code , signal) => {
            console.log(`child process exited with code ${code}`);
            clearTimeout( timer );
            fs.unlinkSync( filename );
        });
        
        var data = await fetch( `https://history.geoworldsim.com/api/scenario/${configuration.id}/socket` , { method : 'POST' , headers : { 'Content-Type': 'application/json' } });
        res.send( configuration );
        
    } catch( err ){
        res.status(500).send( err );
        console.log( 'Error launching simulation' , err );
    };
    
});

/**********************************************************************/
// START
/**********************************************************************/

app.listen(PORT, HOST);
console.log(`[SERVER.JS] Running on http://${HOST}:${PORT}`);













