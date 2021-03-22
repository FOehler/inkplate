class WeatherData {
  public: 
    WeatherData() {
      currentId = -1; 
      currentTemp = -1; 
      nextDayId = -1; 
      nextDayTemp = -1; 
    }

    int currentId; 
    int currentTemp; 

    int nextDayId; 
    int nextDayTemp; 
    String nextDayWeekday; 

    int hourlyHourOfDay[4]; 
    int hourlyId[4]; 
    int hourlyTemp[4]; 
};