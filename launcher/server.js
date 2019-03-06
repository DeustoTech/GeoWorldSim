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

app.post('/api/simulation' , (req, res) => {
   
    let target = req.body.target;
    let config = req.body.config;
    let timeout = req.body.timeout || 60;
    let name = req.body.name || 'New simulation';
    let description = req.body.description;
    let user_id = req.body.user_id;

    const fetch = require('node-fetch');
    const { spawn } = require('child_process');
    let scenario = false;
        
    fetch( `https://history.geoworldsim.com/api/scenario?user_id=${user_id}` , { method : 'POST' , headers : { 'Content-Type': 'application/json' } , body : JSON.stringify( { name : name , description : description , status : "running" } ) })
    .then( res => res.json() )
    .then( json => {
        
            scenario = json;
            return fetch( `https://history.geoworldsim.com/api/scenario/${scenario.id}/socket` , { method : 'POST' , headers : { 'Content-Type': 'application/json' } });
    })
    .then( res => res.text() )
    .then( text => {
        
            let child = spawn( `${__dirname}/targets/${target}` , [ `id=${scenario.id}`, `user_id=${user_id}`, `config=${JSON.stringify(config)}` ] );
            let timer = setTimeout( child.kill , timeout * 1000 );
            child.on('exit', (code , signal) => {
                console.log(`child process exited with code ${code}`);
                clearTimeout( timer );
            });
            
    })
    .then( data => {
        res.send(scenario);
    })
    .catch( err => {
        res.status(500).send( err );
        console.log( 'Error launching simulation' , err );
    });
    
});

/**********************************************************************/
// START
/**********************************************************************/

app.listen(PORT, HOST);
console.log(`[SERVER.JS] Running on http://${HOST}:${PORT}`);













