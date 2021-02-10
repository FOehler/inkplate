#include <string> 
#include "HTTPClient.h"                 // Include library for HTTPClient
#include "Inkplate.h"                   // Include Inkplate library to the sketch
#include "Network.h"
#include "D:/git/inkplate/Fonts/Lato_Light40pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light30pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Light20pt7b.h"           // https://rop.nl/truetype2gfx/
#include "D:/git/inkplate/Fonts/Lato_Regular40pt7b.h"         // Include second font
#include "D:/git/inkplate/Fonts/Lato_Regular30pt7b.h"         // Include second font
#include "D:/git/inkplate/Fonts/Lato_Regular20pt7b.h"         // Include second font
#include "D:/git/inkplate/Fonts/Lato_Bold40pt7b.h"            // Include second font
#include "D:/git/inkplate/Fonts/Lato_Bold30pt7b.h"            // Include second font
#include "D:/git/inkplate/Fonts/Lato_Bold20pt7b.h"            // Include second font

// ---------- Configuration -------------:

// Time zone for adding hours
int timeZone = 1;

// Change to your wifi ssid and password
const char *ssid = "WN-849E40";
const char *pass = "3928253b3b";

// ----------------------------------

#define DELAY_MS 15000

Inkplate display(INKPLATE_1BIT);        // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

Network network;

long refreshes = 0;
const int fullRefresh = 10;

char currentTime[16] = "9:41";
char dateStr[25] = ""; 

void setup() {
    Serial.begin(115200); 
    Serial.print(78);

    network.begin(); 

    Serial.print("PreInit");

    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.setFont(&Lato_Regular20pt7b);
    display.setTextSize(1); 
    display.setCursor(10, 400);
    display.print("22:02");
    display.setFont(&Lato_Light20pt7b);
    display.print(" Montag 08. Februar");

    display.display();
}

void loop() {
    network.getTime(currentTime);
    network.getDateStr(dateStr);

    display.clearDisplay(); // Clear frame buffer of display

    drawTitle();    

    // Refresh full screen every fullRefresh times, defined above
    if (refreshes % fullRefresh == 0)
        display.display();
    else
        display.partialUpdate();

    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
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