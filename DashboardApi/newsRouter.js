/* ============================== News Router ============================== */

const express = require('express');       
const https = require('https')
let Parser = require('rss-parser');
let parser = new Parser();

const apiKey =  "8bcf4df0192ac7f1382e90eaccb09586"; 
const url = "https://www.srf.ch/news/bnf/rss/1646"; 
var returnData = {}; 


const newsRouter = express.Router();
newsRouter.get('/', async (req, res, next) => {
    try {
      let feed = await parser.parseURL(url);
            
      returnData["news"] = []; 
      for (i = 0; i < 10; i++) {
        returnData["news"].push(feed.items[i].title);
      }

      res.json(returnData); 

      // const url = "https://api.openweathermap.org/data/2.5/onecall?lat=" + location_lat + "&lon=" + location_lon + "&appid=" + apiKey + "&units=metric&exclude=minutely,alerts";
      // https.get(url, (response) => {
      //   var resBody = '';
      //   response.on("data", (data) => {
      //     resBody += data; 
          
      //   })
      //   response.on('end', () => {
      //     weatherData = JSON.parse(resBody);
      //     returnData["current"] = {
      //       id: weatherData.current.weather[0].id, 
      //       temp: Math.round(weatherData.current.temp)}
      //     returnData["nextDay"] = {
      //       id: weatherData.daily[1].weather[0].id, // next day is 1, current day 0
      //       temp: Math.round(weatherData.daily[1].temp.day)
      //     }
      //     returnData["hourly"] = []; 
      //     const hourOffset = 4; 
      //     for(i = 0; i < 4; i++) {
      //       var date = new Date(weatherData.hourly[hourOffset + i * 4].dt * 1000);
      //       returnData["hourly"].push({
      //         hourOfDay: date.getHours(),
      //         id: weatherData.hourly[hourOffset + i * 4].weather[0].id,
      //         temp: Math.round(weatherData.hourly[hourOffset + i * 4].temp)
      //       })
      //     }
      //     res.json(returnData); 
      //   })          
      // })
     
      await new Promise((resolve, reject) => {
        setTimeout(() => resolve("done!"), 1000)
      }); 
      next();
    }
    catch (error) {
        next(error);
    }
});

module.exports = newsRouter; 