#include <string> 
#include "NTPClient.h"
#include "WiFiUdp.h"

class TimeHandler {
public:
    const long utcOffsetInSeconds = 3600;

    TimeHandler() {};
    ~TimeHandler() {};    
    

    void Initialise() {
        WiFiUDP ntpUDP;
        this->_timeClient = new NTPClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
        this->_timeClient->begin();
        Serial.print("TimeHandler initialised"); 
    }

    String GetTime() {
        Serial.print("TimeHandler GetTime called"); 
        this->_timeClient->update();
        Serial.print("TimeHandler updated"); 
        int hours = this->_timeClient->getHours(); 
        int minutes = this->_timeClient->getMinutes();
        String timeString = String(hours); // + ":" + String(minutes);
        Serial.print(timeString); 
        return timeString;
    }    

private: 
    NTPClient *_timeClient;
};