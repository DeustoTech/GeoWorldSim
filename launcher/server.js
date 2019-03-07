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
   
    let configuration = req.body;
    let target = configuration.target;
    let timeout = configuration.timeout || 60;
    let name = configuration.name || 'New simulation';
    let description = configuration.description;
    let user_id = configuration.user_id;

    
    if( !target || !user_id ){
        return res.status(404).send();
    }

    const {promisify} = require('util');
    const fetch = require('node-fetch');
    const { spawn } = require('child_process');
    const fs = require('fs');
    let scenario = false;
    let filename = false;

    fetch( `https://history.geoworldsim.com/api/scenario?user_id=${user_id}` , { method : 'POST' , headers : { 'Content-Type': 'application/json' } , body : JSON.stringify( { name : name , description : description , status : "running" } ) })
    .then( res => res.json() )
    .then( json => {
       
        if( !json || !json.id ){
            throw { errors: [ { path: 'scenario', message: 'NOT_CREATED' } ] };
        }

        scenario = json;
        configuration.id = scenario.id;
        filename = `${__dirname}/${scenario.id}.json`;
	
 	return new Promise( (resolve , reject) => {
            var config = JSON.stringify(configuration);
           fs.writeFileSync( filename , config , 'utf8');
           resolve();
        });
    })
    .then( f => {

	    var config_string = `config_file=${filename}`;
	    
        let child
        try {
            console.log( `${__dirname}/targets/${target}`, [ config_string ] );
            child = spawn( `${__dirname}/targets/${target}`, [ config_string ] );
        } catch( err ){ console.log(1 , err) }
        

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
            
    })
    .then( data => {
    
        return fetch( `https://history.geoworldsim.com/api/scenario/${scenario.id}/socket` , { method : 'POST' , headers : { 'Content-Type': 'application/json' } });
    })
    .then( res => res.text() )
    .then( text => {
        res.send( scenario );
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













