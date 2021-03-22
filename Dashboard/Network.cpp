#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <string> 
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

char weekDays[7][7] = {
    "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"
};

char weekDaysLong[7][12] = {
    "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag", "Sonntag"
};

char months[12][12] = {
    "Januar", "Februar", "Maerz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"
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
    Serial.println("Getting Time"); 
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
    Serial.println("Getting Date String"); 
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
    
    std::string stringCopy  = std::string(weekDaysLong[dayWeek]) + ' ' + monthChar + '.' + ' ' + months[month]; 
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

bool Network::getCalendarItems(CalendarDay* calendarData[6])
{
    Serial.println("Getting Calendar Data"); 
    bool f = 0;
    HTTPClient http;

    http.useHTTP10(true);
    http.begin(calendarApi);
    Serial.println("Calendar Api configured to: ");
    Serial.println(calendarApi);
    delay(300);

    // Actually do request
    int httpCode = http.GET();

    Serial.println("httpCode");
    if (httpCode == 200)
    {
        Serial.println("Data received..."); 
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        int lastMonthDay = -1; 
        int k = 0; 
        calendarData[0]->daysToDisplay = 0; 
        for(int i = 0; i < 6; i++) {
            struct tm tmStart, tmEnd;
            String titleString = doc["events"][i]["title"].as<String>(); 
            Serial.println("Retreiving event: "  + titleString); 
            
            String startString = doc["events"][i]["start"].as<String>(); 
            String timeString = " "; 
            if (startString.length() == 10) {
                strptime(startString.c_str(), "%Y-%m-%d", &tmStart);
                timeString = ""; 
            } else {
                String endString = doc["events"][i]["end"].as<String>(); 
                strptime(endString.c_str(), "%Y-%m-%dT%H:%M:%S", &tmEnd);
                strptime(startString.c_str(), "%Y-%m-%dT%H:%M:%S", &tmStart);
                String startMin = String(tmStart.tm_min); 
                if (startMin.length() == 1) {
                    startMin = "0" + startMin; 
                }
                String endMin = String(tmEnd.tm_min); 
                if (endMin.length() == 1) {
                    endMin = "0" + endMin; 
                }
                timeString = String(tmStart.tm_hour) + ":" + startMin + " - " + String(tmEnd.tm_hour) + ":" + endMin; 
            }
            String weekDayString = String(weekDays[tmStart.tm_wday]); 
            String monthDayString; 
            monthDayString = String(tmStart.tm_mday) + "."; 
            if (lastMonthDay == tmStart.tm_mday) {
                Serial.println("Same Day Event: "  + titleString); 
                k += 1; 
                calendarData[i-1]->time[k] = timeString; 
                calendarData[i-1]->title[k] = titleString; 
            }
            else {
                k = 0; 
                calendarData[0]->daysToDisplay += 1; 
                calendarData[i]->day = weekDayString; 
                calendarData[i]->date = monthDayString; 
                calendarData[i]->time[k] = timeString; 
                calendarData[i]->title[k] = titleString; 
            }
            lastMonthDay = tmStart.tm_mday; 
        }
        Serial.println("Data stored to array..."); 
        long n = 0;
    }
    else
    {
        Serial.println("Calendar Request failed, HTTP Code = ");
        Serial.println(httpCode);
        f = 1;
    }

    // end http
    http.end();

    return !f;
}

bool Network::getWeatherData(WeatherData* weatherData)
{
    Serial.println("Getting Weather Data"); 
    bool f = 0;
    HTTPClient http;

    http.useHTTP10(true);
    http.begin(weatherApi);
    Serial.println("Weather Api configured to: ");
    Serial.println(weatherApi);
    delay(300);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        Serial.println("Data received..."); 
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        weatherData->currentId = doc["current"]["id"].as<int>();
        weatherData->currentTemp = doc["current"]["temp"].as<int>();
        weatherData->nextDayId = doc["nextDay"]["id"].as<int>();
        weatherData->nextDayTemp = doc["nextDay"]["temp"].as<int>();
        for (int i = 0; i < 4; i++) {
            weatherData->hourlyHourOfDay[i] = doc["hourly"][i]["hourOfDay"].as<int>(); 
            weatherData->hourlyId[i] = doc["hourly"][i]["id"].as<int>(); 
            weatherData->hourlyTemp[i] = doc["hourly"][i]["temp"].as<int>(); 
        }
        Serial.println("Weather data stored..."); 
        long n = 0;
    }
    else
    {
        Serial.println("Weather Request failed, HTTP Code = ");
        Serial.println(httpCode);
        f = 1;
    }

    // end http
    http.end();

    // get next week day
    time_t nowSecs = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);
    int dayWeek = ((long)((nowSecs + 3600L * timeZone) / 86400L) + 3) % 7;
    int nextDayWeek = dayWeek + 2; 
    if (nextDayWeek == 7) {
        nextDayWeek = 0; 
    }
    if (nextDayWeek == 8) {
        nextDayWeek = 1; 
    }
    weatherData->nextDayWeekday = weekDays[nextDayWeek]; 

    return !f;
}