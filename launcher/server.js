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
        var child = spawn( `${__dirname}/targets/${configuration.target}` , [ filename ] , { cwd : `${__dirname}/targets` } );
        
        // SET TIMEOUT
        let timer = setTimeout( function(){ 
            console.log( `Timeout for Simulation ${configuration.target}:${configuration.id}` );
            child.kill();
        } , (configuration.timeout * 1000) || 600000 );
        
        console.log( `Simulation ${configuration.target}:${configuration.id} started. Will be killed in ${(configuration.timeout || 600) / 3600} hours` )
        
        var access = fs.createWriteStream( `${__dirname}/logs/${configuration.id}_console.log` , { flags: 'a' })
        , error = fs.createWriteStream( `${__dirname}/logs/${configuration.id}_error.log` , { flags: 'a' });
        
        child.stdout.pipe(access);
        child.stderr.pipe(error);
        
        // PROCESS FINISHED OR CRASHED
        child.on('exit', (code , signal) => {
            
            console.log( `Simulation ${configuration.target}:${configuration.id} exited with code ${code}` );

            fetch( `https://history.geoworldsim.com/api/scenario/${configuration.id}/status` , { method : 'PUT' , headers : { 'Content-Type': 'application/json' } , body : JSON.stringify({ status : (code != 0 ? 'crashed' : 'finished') }) })
            .then( () => {
                    clearTimeout( timer );
                    fs.unlinkSync( filename );
            })
            .catch( err => {});
            
            
            
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













