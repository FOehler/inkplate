#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <string> 

#include <ArduinoJson.h>

char weekDays[12][12] = {
    "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag",
};

char months[12][12] = {
    "Januar", "Februar", "M\x84rz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"
};


void Network::begin() 
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int cnt = 0;
    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(F("."));
        delay(1000);
        ++cnt;

        if (cnt == 20)
        {
            Serial.println("Can't connect to WIFI, restarting");
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));

    // Find internet time
    setTime();
}

void Network::getTime(char *timeStr)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    // Copies time string into timeStr
    strncpy(timeStr, asctime(&timeinfo) + 11, 5);

    // Setting time string timezone
    int hr = 10 * timeStr[0] + timeStr[1] + timeZone;

    // Better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // Adding time to '0' char makes it into whatever time char, for both digits
    timeStr[0] = hr / 10 + '0';
    timeStr[1] = hr % 10 + '0';
}

void Network::getDateStr(char *dateStr) 
{
    // Seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    int dayWeek = ((long)((nowSecs + 3600L * timeZone) / 86400L) + 3) % 7;
    int monthDay = timeinfo.tm_mday;
    char monthChar[2 + sizeof(char)];
    std::sprintf(monthChar, "%d", monthDay);
    int month = timeinfo.tm_mon; 
    
    std::string stringCopy  = std::string(weekDays[dayWeek]) + ' ' + monthChar + '.' + ' ' + months[month]; 
    // Copy day data to globals in main file
    strncpy(dateStr, stringCopy.c_str(), 25);
}

void Network::setTime()
{
    // Used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        // Print a dot every half a second while time is not set
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }

    Serial.println();

    // Used to store time info
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}