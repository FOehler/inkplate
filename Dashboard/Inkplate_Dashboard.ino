#include <SHT21.h>

#include "Inkplate.h"                   // Include Inkplate library to the sketch
#include "Network.h"

#include "D:/git/inkplate/Fonts/Lato_Light40pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light30pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light20pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light15pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light10pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular40pt7b.h"         // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular30pt7b.h"         // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular20pt7b.h"         // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular15pt7b.h"         // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular10pt7b.h"         // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Bold40pt7b.h"            // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Bold30pt7b.h"            // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Bold20pt7b.h"            // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Bold15pt7b.h"            // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Bold10pt7b.h"            // https://rop.nl/truetype2gfx/

#include <string> 

// ---------- Configuration -------------:

// Time zone for adding hours
int timeZone = 1;

// Change to your wifi ssid and password
const char *ssid = "WN-849E40";
const char *pass = "3928253b3b";

const char *calendarApi = "http://192.168.0.25:8080/calendar";
const char *weatherApi = "http://192.168.0.25:8080/weather";
const char *newsApi = "http://192.168.0.25:8080/news";

// ----------------------------------

#include "icons.h"

#define DELAY_MS 5000
#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  50      // How long ESP32 will be in deep sleep (in seconds)
#define TIME_TO_SLEEP_OVER_NIGHT  1200      // How long ESP32 will be in deep sleep (in seconds)

Inkplate display(INKPLATE_1BIT);        // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

Network network;

long refreshes = 0;
const int fullRefresh = 10;

CalendarDay* calendarData[6];
WeatherData* weatherData; 
NewsData* newsData; 
SHT21 sht; 


char currentTime[16] = "9:41";
char dateStr[25] = ""; 
char news[12][40] = {
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
    "0F",
};
const uint8_t *s_logos[16] = {icon_s_sn, icon_s_sl, icon_s_h, icon_s_t, icon_s_hr, icon_s_lr, icon_s_s, icon_s_hc, icon_s_lc, icon_s_c};


void drawTitle();
void retrieveCalendarData(); 
void drawCalendar();
void retrieveWeatherData(); 
void retrieveNewsData(); 
void drawWeather();
void drawNews();
void drawVoltage(); 
void drawInteriorTemp(); 
int drawDay(int offset, int iterator);
int getIconIdFromWeatherId(int weatherId);
void drawNewsItem(int offset); 

void setup() {
    Wire.begin();
    Serial.begin(115200); 
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)

    for(size_t i = 0; i != 6; ++i)
    {
        calendarData[i] = new CalendarDay(); 
    }
    weatherData = new WeatherData(); 
    newsData = new NewsData(); 

    network.begin(); 
    network.getTime(currentTime);
    network.getDateStr(dateStr);

    retrieveCalendarData();
    retrieveWeatherData(); 
    retrieveNewsData(); 

    display.clearDisplay(); // Clear frame buffer of display
    drawTitle(); 
    drawCalendar(); 
    drawWeather(); 
    drawNews();
    drawVoltage(); 
    drawInteriorTemp(); 
    display.display();

    if (network.currentHour > 23 && network.currentHour < 6) {
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_OVER_NIGHT * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    }
    else {
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer -- wake up after 20s here
    }
    esp_deep_sleep_start();      
}

void loop() {
    // network.getTime(currentTime);
    // network.getDateStr(dateStr);

    // display.clearDisplay(); // Clear frame buffer of display

    // drawTitle(); 
    // drawCalendar(); 
    
    // drawWeather(); 
    // drawNews(); 

    // drawVoltage(); 

    // drawInteriorTemp(); 

    // // Refresh full screen every fullRefresh times, defined above
    // if (refreshes % fullRefresh == 0)
    //     display.display();
    // else
    //     display.partialUpdate();

    // esp_sleep_enable_timer_wakeup(5000L * DELAY_MS);
    // (void)esp_light_sleep_start();
    // ++refreshes;
}

void drawTitle() {
    display.setFont(&Lato_Bold30pt7b);
    display.setTextSize(1); 
    display.setCursor(10, 60);
    display.print(currentTime);
    display.setFont(&Lato_Light30pt7b);
    display.print(" "); 
    display.print(dateStr);
}

void drawCalendar() {
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(10, 120);
    display.print("Diese");
    display.setFont(&Lato_Light20pt7b);
    display.print(" Woche"); 
  
    int offset = 180; 
    for (int i = 0; i < calendarData[0]->daysToDisplay; i++) {
        int additionalOffset = drawDay(offset, i); 
        offset += additionalOffset; 
    }
}

void retrieveCalendarData() {
    while (!network.getCalendarItems(calendarData))
    {
        Serial.println("Failed getting data, retrying");
        display.print("Could not reach Calendar API!");
        delay(1000);
    }
}

void drawWeather() {
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(400, 120);
    display.print("Wetter");
    // Current Day
    display.setFont(&Lato_Bold30pt7b);
    display.setCursor(460, 185);
    String tempString = String(weatherData->currentTemp); 
    display.print(tempString + " C");
    display.setFont(&Lato_Bold10pt7b);
    display.setCursor(460 + tempString.length() * 35, 155);
    display.print("o");
    display.drawBitmap(400, 140, s_logos[getIconIdFromWeatherId(weatherData->currentId)], 48, 48, BLACK, WHITE);
    // Hourly
    int offset = 80; 
    int iconSize = 48; 
    int titleOffset = 6; 
    for (int i = 0; i < 5; i++) {
        int horizontalPosition = 400 + offset * i; 
        if (i < 4) {
            display.setFont(&Lato_Bold15pt7b);
            display.setCursor(horizontalPosition + titleOffset, 220);
            String hourString = String(weatherData->hourlyHourOfDay[i]); 
            if (hourString.length() == 1) {
                hourString = "0" + hourString; 
            }
            display.print(hourString); 
            display.drawBitmap(horizontalPosition, 230, s_logos[getIconIdFromWeatherId(weatherData->hourlyId[i])], iconSize, iconSize, BLACK, WHITE);
            display.setFont(&Lato_Light15pt7b);
            display.setCursor(horizontalPosition, 310);
            String tempString = String(weatherData->hourlyTemp[i]); 
            if (tempString.length() == 1) {
                tempString = " " + tempString; 
            }
            display.print(tempString + "C");
        }
        else {
            display.setFont(&Lato_Bold15pt7b);
            display.setCursor(horizontalPosition + titleOffset, 220);
            display.print(weatherData->nextDayWeekday); 
            display.drawBitmap(horizontalPosition, 230, s_logos[getIconIdFromWeatherId(weatherData->nextDayId)], iconSize, iconSize, BLACK, WHITE);
            display.setFont(&Lato_Light15pt7b);
            display.setCursor(horizontalPosition, 310);
            String tempString = String(weatherData->nextDayTemp); 
            if (tempString.length() == 1) {
                tempString = " " + tempString; 
            }
            display.print(tempString + "C");
        }
    }
}

void retrieveWeatherData() {
    while (!network.getWeatherData(weatherData)) {
        Serial.println("Failed getting data, retrying");
        display.print("Could not reach Calendar API!");
        delay(1000);
    }
}

int getIconIdFromWeatherId(int weatherId) {
    // {"sn", "sl", "h",   "t",      "hr",  "lr",      "s",     "hc",      "lc",    "c"};
    //   0     1     2      3         4      5          6        7          8        9
    // snow   sleet hail thunder heavyRain lightRain Showers heavyCloud lightCloud Clear
    if (weatherId < 300) {
        return 3; 
    }
    if (weatherId < 501) {
        return 5; 
    }
    if (weatherId < 511) {
        return 4; 
    }
    if (weatherId < 600) {
        return 6; 
    }
    if (weatherId == 611) {
        return 1; 
    }
    if (weatherId < 700) {
        return 0; 
    }
    if (weatherId < 800) {
        return 8; 
    }
    if (weatherId == 800) {
        return 9;
    }
    if (weatherId < 803) {
        return 8; 
    }
    if (weatherId < 805) {
        return 7; 
    }
}

int drawDay(int topOffset, int iterator) {
    int offsetPerEvent = 80; 

    if (calendarData[iterator]->title[0].length() > 3) {
        display.setCursor(10, topOffset);
        display.setFont(&Lato_Bold30pt7b);
        display.setTextSize(1); 
        display.print(calendarData[iterator]->day);
        display.setCursor(25, topOffset + 40);
        display.setFont(&Lato_Light15pt7b);
        display.setTextSize(1); 
        display.print(calendarData[iterator]->date);
    }

    int leftOffset = 120;
    int k = 0; 
    for (int i = 0; i < 6; i++) {
        if (calendarData[iterator]->title[i].length() > 3) {
            display.setCursor(leftOffset, topOffset - 20 + k * 70);
            display.setFont(&Lato_Bold15pt7b);
            display.setTextSize(1); 
            display.print(calendarData[iterator]->title[i]);
            display.setCursor(leftOffset, topOffset + 15 + k * 70);
            display.setFont(&Lato_Light15pt7b);
            display.setTextSize(1); 
            display.print(calendarData[iterator]->time[i]);
            k += 1; 
        }
    }
    if (k == 1) { // If there is only one event, a bigger vertical offset is required
        return k * offsetPerEvent + 15; 
    }
    return k * offsetPerEvent; 
}

void drawNews() {
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(400, 360);
    display.print("News");
    drawNewsItem(390); 
}

void drawNewsItem(int offset) {
    int lineSpace = 20; 
    int lengthCutoff = 400; 
    display.setFont(&Lato_Light10pt7b);
    display.setTextSize(1); 
    int lineNumber = 0;
    int newsIterator = 0; 
    while(lineNumber < 10) {
        display.setCursor(400, offset + lineNumber * lineSpace);
        display.print("- "); 
        for (int k = 0; k < newsData->newsLineNumbers[newsIterator]; k++) {
            display.print(newsData->newsText[newsIterator][k]); 
            lineNumber++; 
            display.setCursor(400, offset + lineNumber * lineSpace);
            if (k < newsData->newsLineNumbers[newsIterator] - 1) {
                display.print("   ");
            }
        }
        newsIterator++; 
    }
}

void retrieveNewsData() {
    while (!network.getNewsData(newsData)) {
        Serial.println("Failed getting data, retrying");
        display.print("Could not reach Calendar API!");
        delay(1000);
    }
}

void drawVoltage() {
    float voltage = display.readBattery();
    display.setFont(&Lato_Light10pt7b);
    display.setCursor(745, 20);
    display.print(voltage, 2);
    display.print('V');
}

void drawInteriorTemp() {
    float temp = sht.getTemperature();  // get temp from SHT 
    float humidity = sht.getHumidity(); // get temp from SHT

    int xStart = 645; 
    int yStart = 185; 
    int houseWidth = 40; 
    int houseHeight = 32; 
    display.drawLine(xStart, yStart, xStart, yStart - houseHeight, BLACK); 
    display.drawLine(xStart, yStart - houseHeight, xStart + houseWidth / 2, yStart - houseHeight - houseHeight / 2, BLACK); 
    display.drawLine(xStart + houseWidth / 2, yStart - houseHeight - houseHeight / 2, xStart + houseWidth, yStart - houseHeight, BLACK); 
    display.drawLine(xStart + houseWidth, yStart - houseHeight, xStart + houseWidth, yStart, BLACK); 
    display.drawLine(xStart + houseWidth, yStart, xStart, yStart, BLACK); 

    display.setFont(&Lato_Light15pt7b);
    display.setCursor(700, 160);
    display.print(temp, 1); 
    display.print('C'); 
    display.setCursor(700, 185);
    display.print(humidity, 1); 
    display.print('%'); 

}