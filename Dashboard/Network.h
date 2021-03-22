#include "Arduino.h"
#include "Models/CalendarDay.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// To get timeZone from main file
extern int timeZone;

// wifi ssid and password
extern const char* ssid;
extern const char* pass;

// apis
extern const char* calendarApi; 

// data holders 
extern CalendarDay* calendarData[6];

#ifndef NETWORK_H
#define NETWORK_H

// All functions defined in Network.cpp

class Network
{
  public:
    // Functions we can access in main file
    void begin();
    void getTime(char *timeStr);
    void getDateStr(char *dateStr); 
    bool getCalendarItems(CalendarDay* calendarData[6]);

    // Used to store loaction woeid (world id), set in findCity()
    int location = -1;

  private:
    // Functions called from within our class
    void setTime();
    void findCity(char *city);
};

#endif
