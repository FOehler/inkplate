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

const char *calendarApi = "http://192.168.0.21:8080/calendar";
const char *weatherApi = "http://192.168.0.21:8080/weather";

// ----------------------------------

#include "icons.h"

#define DELAY_MS 5000

Inkplate display(INKPLATE_1BIT);        // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

Network network;

long refreshes = 0;
const int fullRefresh = 10;

CalendarDay* calendarData[6];
WeatherData* weatherData; 

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
void drawCalendar();
void drawWeather();
void drawNews();
void drawVoltage(); 
int drawDay(int offset, int iterator);
int getIconIdFromWeatherId(int weatherId);
void drawNewsItem(char* news, int offset); 

void setup() {
    Serial.begin(115200); 

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    for(size_t i = 0; i != 6; ++i)
    {
        calendarData[i] = new CalendarDay(); 
    }
    weatherData = new WeatherData(); 

    delay(2000);
    network.begin(); 
}

void loop() {
    network.getTime(currentTime);
    network.getDateStr(dateStr);

    display.clearDisplay(); // Clear frame buffer of display

    drawTitle(); 
    drawCalendar(); 
    
    drawWeather(); 
    drawNews(); 

    drawVoltage(); 

    // Refresh full screen every fullRefresh times, defined above
    if (refreshes % fullRefresh == 0)
        display.display();
    else
        display.partialUpdate();

    esp_sleep_enable_timer_wakeup(5000L * DELAY_MS);
    (void)esp_light_sleep_start();
    ++refreshes;
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

    while (!network.getCalendarItems(calendarData))
    {
        Serial.println("Failed getting data, retrying");
        display.print("Could not reach Calendar API!");
        delay(1000);
    }
    int offset = 180; 
    for (int i = 0; i < calendarData[0]->daysToDisplay; i++) {
        int additionalOffset = drawDay(offset, i); 
        offset += additionalOffset; 
    }
}

void drawWeather() {
    while (!network.getWeatherData(weatherData)) {
        Serial.println("Failed getting data, retrying");
        display.print("Could not reach Calendar API!");
        delay(1000);
    }
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(400, 120);
    display.print("Wetter");
    // Current Day
    display.setFont(&Lato_Bold30pt7b);
    display.setCursor(460, 185);
    String tempString = String(weatherData->currentTemp); 
    display.print(tempString + "C");
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
            display.print(tempString + "C");
        }
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
    strncpy(news[0], "- Rund um den Globus fordern heute die ", sizeof(news[0]));
    strncpy(news[1], "   Frauen mehr Gerechtigkeit", sizeof(news[1]));
    strncpy(news[2], "- Die UBS hofft in Paris auf gnädige  ", sizeof(news[0]));
    strncpy(news[3], "   Richter", sizeof(news[1]));
    drawNewsItem(news[0], 390); 
    drawNewsItem(news[1], 420); 
    drawNewsItem(news[2], 450); 
    drawNewsItem(news[3], 480); 
}

void drawNewsItem(char* newsText, int offset) {
    display.setFont(&Lato_Light10pt7b);
    display.setTextSize(1); 
    display.setCursor(400, offset);
    display.print(newsText);
}

void drawVoltage() {
    float voltage = display.readBattery();
    display.setFont(&Lato_Light10pt7b);
    display.setCursor(745, 20);
    display.print(voltage, 2);
    display.print('V');
}