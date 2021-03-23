/* ============================== News Router ============================== */

const express = require('express');       
const https = require('https')
let Parser = require('rss-parser');
let parser = new Parser();

const apiKey =  "8bcf4df0192ac7f1382e90eaccb09586"; 
const url = "https://www.srf.ch/news/bnf/rss/1646"; 
var returnData = {}; 

const lengthCutoff = 40; 

const newsRouter = express.Router();
newsRouter.get('/', async (req, res, next) => {
    try {
      console.log('Received a News API call!');
      let feed = await parser.parseURL(url);
      returnData["news"] = []; 
      for (i = 0; i < 10; i++) {
        var newsLines = {}; 
        newsLines = [];
        var newsItem = feed.items[i].title; 
        newsItem = newsItem.replace(/\u00e4/g, "ae");
        newsItem = newsItem.replace(/\u00c4/g, "Ae");
        newsItem = newsItem.replace(/\u00f6/g, "oe");
        newsItem = newsItem.replace(/\u00d6/g, "Oe");
        newsItem = newsItem.replace(/\u00fc/g, "ue");
        newsItem = newsItem.replace(/\u00dc/g, "Ue");
        console.log('Return news item: ' + newsItem)
        while (true) {
          var cutString = shorten(newsItem, lengthCutoff);
          newsLines.push(cutString); 
          if (cutString != newsItem) {
            newsItem = newsItem.substring(cutString.length+1, newsItem.length ); 
          }
          else {
            break;
          }
        }
        returnData["news"].push(newsLines);
      }
      console.log(returnData);
      res.json(returnData); 

     
      await new Promise((resolve, reject) => {
        setTimeout(() => resolve("done!"), 1000)
      }); 
      next();
    }
    catch (error) {
        next(error);
    }
});

function shorten(str, maxLen, separator = ' ') {
  if (str.length <= maxLen) return str;
  return str.substr(0, str.lastIndexOf(separator, maxLen));
}

module.exports = newsRouter; 