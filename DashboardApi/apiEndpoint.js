var express = require('express');        
var app = express();                 
var bodyParser = require('body-parser');
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
var port = process.env.PORT || 8080;  

// TODO: Behaviour when one calendar has no events

var calendarRouter = require('./calendarRouter');
app.use('/calendar', calendarRouter);

var weatherRouter = require('./weatherRouter');
app.use('/weather', weatherRouter);

var newsRouter = require('./newsRouter');
app.use('/news', newsRouter);

app.listen(port);
console.log('Listening on port ' + port);
