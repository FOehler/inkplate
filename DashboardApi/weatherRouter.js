/* ============================== Calendar Router ============================== */

const express = require('express');       
const https = require('https')

const location_lon = "8.6989"; 
const location_lat = "47.2569"; 
const apiKey =  "8bcf4df0192ac7f1382e90eaccb09586"; 

var weatherData;
var returnData = {}; 


const weatherRouter = express.Router();
weatherRouter.get('/', async (req, res, next) => {
    try {
      console.log('Received a Weather API call!');
      const url = "https://api.openweathermap.org/data/2.5/onecall?lat=" + location_lat + "&lon=" + location_lon + "&appid=" + apiKey + "&units=metric&exclude=minutely,alerts";
      https.get(url, (response) => {
        var resBody = '';
        response.on("data", (data) => {
          resBody += data; 
          
        })
        response.on('end', () => {
          weatherData = JSON.parse(resBody);
          returnData["current"] = {
            id: weatherData.current.weather[0].id, 
            temp: Math.round(weatherData.current.temp)}
          returnData["nextDay"] = {
            id: weatherData.daily[1].weather[0].id, // next day is 1, current day 0
            temp: Math.round(weatherData.daily[1].temp.day)
          }
          returnData["hourly"] = []; 
          const hourOffset = 4; 
          for(i = 0; i < 4; i++) {
            var date = new Date(weatherData.hourly[hourOffset + i * 4].dt * 1000);
            returnData["hourly"].push({
              hourOfDay: date.getHours(),
              id: weatherData.hourly[hourOffset + i * 4].weather[0].id,
              temp: Math.round(weatherData.hourly[hourOffset + i * 4].temp)
            })
          }
          console.log(returnData);
          res.json(returnData); 
        })          
      })
     
      await new Promise((resolve, reject) => {
        setTimeout(() => resolve("done!"), 1000)
      }); 
      next();
    }
    catch (error) {
        next(error);
    }
});

module.exports = weatherRouter; 