/* ============================== Calendar Router ============================== */

var express = require('express');        
var nextPrimaryEvents; 
var nextSandraEvents;
var nextEntsorgungEvents;
var oauthClient; 

class calendarEvent {
  constructor(title, startDate, endDate) {
    this.title = title;
    this.startDate = startDate;
    this.endDate = endDate;
  }
}

const calendarRouter = express.Router();
calendarRouter.get('/', async (req, res, next) => {
    try {
      var current = new Date();
      console.log('Received a Calendar API call @' + current.toISOString());
      getEvents(oauthClient);
      // create a timeout
      await new Promise((resolve, reject) => {
        setTimeout(() => resolve("done!"), 1500)
      }); 
      res.json(getJson(nextPrimaryEvents.concat(nextEntsorgungEvents).concat(nextSandraEvents)));  
      next();
    }
    catch (error) {
        next(error);
    }
});

function getJson(nextEvents) {
  calendarEvents = [];
  for (i = 0; i < nextEvents.length; i++) {
    currentEvent = nextEvents[i]; 
    if (currentEvent != undefined) {
      if (currentEvent.start.date == undefined || currentEvent.start == undefined) {
        startDate = currentEvent.start.dateTime
      } else {
        startDate = currentEvent.start.date
      }
      if (currentEvent.end.date == undefined || currentEvent.end == undefined) {
        endDate = currentEvent.end.dateTime
      } else {
        endDate = currentEvent.end.date
      }
      calendarEvents.push(new calendarEvent(currentEvent.summary, startDate, endDate))
    }    
    
  }
  calendarEvents.sort( (a, b) => Date.parse(a.startDate) - Date.parse(b.startDate) )
  var text = `{ "events" : [`;
  for(i = 0; i < calendarEvents.length; i++) {
    currentEvent = calendarEvents[i]; 
    text = text + `{"title":"${currentEvent.title}", `;
    text = text + `"start":"${currentEvent.startDate}",`;
    text = text + `"end":"${currentEvent.endDate}"`
    text = text + `  }`;                       
    if (i != nextEvents.length-1) {
      text = text + ","
    }
  }
  text = text + `]}`
  console.log(text);
  return JSON.parse(text)
}

// Google Calendar Logic-----------------------------------------------------
const fs = require('fs');
const readline = require('readline');
const {google} = require('googleapis');
const { start } = require('repl');
const SCOPES = ['https://www.googleapis.com/auth/calendar.readonly'];
const TOKEN_PATH = 'token.json';

fs.readFile('credentials.json', (err, content) => {
    if (err) return console.log('Error loading client secret file:', err);
    // Authorize a client with credentials, then call the Google Calendar API.
    authorize(JSON.parse(content));
});

/**
 * Create an OAuth2 client with the given credentials, and then execute the
 * given callback function.
 * @param {Object} credentials The authorization client credentials.
 * @param {function} callback The callback to call with the authorized client.
 */
const authorize = async function(credentials) {
  const {client_secret, client_id, redirect_uris} = credentials.installed;
  const oAuth2Client = new google.auth.OAuth2(
      client_id, client_secret, redirect_uris[0]);
  // Check if we have previously stored a token.
  fs.readFile(TOKEN_PATH, (err, token) => {
    if (err) return getAccessToken(oAuth2Client, callback);
    oAuth2Client.setCredentials(JSON.parse(token));
    oauthClient = oAuth2Client;
  });
}

/**
 * Get and store new token after prompting for user authorization, and then
 * execute the given callback with the authorized OAuth2 client.
 * @param {google.auth.OAuth2} oAuth2Client The OAuth2 client to get token for.
 * @param {getEventsCallback} callback The callback for the authorized client.
 */
function getAccessToken(oAuth2Client, callback) {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: 'offline',
    scope: SCOPES,
  });
  console.log('Authorize this app by visiting this url:', authUrl);
  const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
  });
  rl.question('Enter the code from that page here: ', (code) => {
    rl.close();
    oAuth2Client.getToken(code, (err, token) => {
      if (err) return console.error('Error retrieving access token', err);
      oAuth2Client.setCredentials(token);
      // Store the token to disk for later program executions
      fs.writeFile(TOKEN_PATH, JSON.stringify(token), (err) => {
        if (err) return console.error(err);
        console.log('Token stored to', TOKEN_PATH);
      });
      callback(oAuth2Client);
    });
  });
}

/**
 * Lists the next 10 events on the user's primary calendar.
 * @param {google.auth.OAuth2} auth An authorized OAuth2 client. 
 */
function getEvents(auth) {
    const calendar = google.calendar({version: 'v3', auth});
    calendar.events.list({
      calendarId: 'primary',
      timeMin: (new Date()).toISOString(),
      maxResults: 10,
      singleEvents: true,
      orderBy: 'startTime',
    }, (err, res) => {
      if (err) return console.log('The API returned an error: ' + err);
      const events = res.data.items;
      if (events.length) {
        nextPrimaryEvents = events; 
      } else {
        console.log('No upcoming events found.');
      }
    });
    calendar.events.list({
      calendarId: '75goj32pkis4p9ltsjvfld8j98@group.calendar.google.com',
      timeMin: (new Date()).toISOString(),
      maxResults: 10,
      singleEvents: true,
      orderBy: 'startTime',
    }, (err, res) => {
      if (err) return console.log('The API returned an error: ' + err);
      const events = res.data.items;
      if (events.length) {
        nextEntsorgungEvents = events
      } else {
        console.log('No upcoming events found.');
      }
    });
    calendar.events.list({
      calendarId: 'sandrabaumann55@gmail.com',
      timeMin: (new Date()).toISOString(),
      maxResults: 10,
      singleEvents: true,
      orderBy: 'startTime',
    }, (err, res) => {
      if (err) return console.log('The API returned an error: ' + err);
      const events = res.data.items;
      if (events.length) {
        nextSandraEvents = events
      } else {
        console.log('No upcoming events found.');
      }
    });
  }
  
  module.exports = calendarRouter; 
