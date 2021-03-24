class CalendarDay {
  public: 
    CalendarDay() {
      day = "Mo"; 
      date = "11."; 
      title[0] = " "; 
      title[1] = " "; 
      title[2] = " "; 
      title[3] = " "; 
      title[4] = " "; 
      title[5] = " "; 
      time[0] = "18:00 - 20:00"; 
    }

    int daysToDisplay; 
    String day; 
    String date; 
    String title[6]; 
    String time[6]; 
};

typedef struct {
    int daysToDisplay; 
    String day; 
    String date; 
    String title[6]; 
    String time[6];  
} calendarDataStruct; 