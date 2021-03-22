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

// ----------------------------------

#include "icons.h"

#define DELAY_MS 5000

Inkplate display(INKPLATE_1BIT);        // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

Network network;

long refreshes = 0;
const int fullRefresh = 10;

CalendarDay* calendarData[6];
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
const uint8_t *logos[16] = {icon_sn, icon_sl, icon_h, icon_t, icon_hr, icon_lr, icon_s, icon_hc, icon_lc, icon_c};
const uint8_t *s_logos[16] = {icon_s_sn, icon_s_sl, icon_s_h, icon_s_t, icon_s_hr, icon_s_lr, icon_s_s, icon_s_hc, icon_s_lc, icon_s_c};


void drawTitle();
void drawCalendar();
void drawWeather();
void drawNews();
int drawDay(int offset, int iterator);
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

    // Refresh full screen every fullRefresh times, defined above
    if (refreshes % fullRefresh == 0)
        display.display();
    else
        display.partialUpdate();

    esp_sleep_enable_timer_wakeup(10000L * DELAY_MS);
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
    network.getCalendarItems(calendarData);
    int offset = 180; 
    for (int i = 0; i < calendarData[0]->daysToDisplay; i++) {
        int additionalOffset = drawDay(offset, i); 
        offset += additionalOffset; 
    }
}

void drawWeather() {
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(400, 120);
    display.print("Wetter");
    display.drawBitmap(400, 180, s_logos[1], 48, 48, BLACK, WHITE);
    display.drawBitmap(500, 180, s_logos[2], 48, 48, BLACK, WHITE);
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
    Serial.println("k = " + String(k)); 
    if (k == 1) { // If there is only one event, a bigger vertical offset is required
        return k * offsetPerEvent + 15; 
    }
    return k * offsetPerEvent; 
}

void drawNews() {
    display.setFont(&Lato_Bold20pt7b);
    display.setTextSize(1); 
    display.setCursor(400, 300);
    display.print("News");
    strncpy(news[0], "- Rund um den Globus fordern heute die ", sizeof(news[0]));
    strncpy(news[1], "   Frauen mehr Gerechtigkeit", sizeof(news[1]));
    strncpy(news[2], "- Die UBS hofft in Paris auf gnädige  ", sizeof(news[0]));
    strncpy(news[3], "   Richter", sizeof(news[1]));
    drawNewsItem(news[0], 330); 
    drawNewsItem(news[1], 360); 
    drawNewsItem(news[2], 390); 
    drawNewsItem(news[3], 420); 
}

void drawNewsItem(char* newsText, int offset) {
    display.setFont(&Lato_Light10pt7b);
    display.setTextSize(1); 
    display.setCursor(400, offset);
    display.print(newsText);
}