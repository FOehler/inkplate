class NewsData {
  public: 
    NewsData() {
      newsText[0][0] = ""; 
      newsLineNumbers[0] = -1;
    }

    int newsLineNumbers[10]; 
    String newsText[10][4]; 
};