
void drawScrollingGraph(Adafruit_SSD1306 &d, int x, int y, int w, int h, int data[], unsigned int dataLength, int minValue, int maxValue) {
  int vRange = (maxValue - minValue);

  float invertedX = x + w;
  float invertedY = (y + h) - 1;
  int graphWidth = w - 10;
  int graphHeight = h - 0;
  float invertedGraphX = x + graphWidth;
  float invertedGraphY = (y + graphHeight) - 1;
  float xScale = (float)graphWidth / (float)dataLength;
  float yScale = (float)graphHeight / (float)vRange;

  d.setTextSize(1);
  d.setCursor(x, invertedY - 6);
  d.println(minValue);
  d.setCursor(x, y);
  d.println(maxValue);
  
  d.drawFastVLine(invertedX - 1, y, h, WHITE);
  d.drawFastHLine(invertedX - 6, y, 5, WHITE);
  
  for (int i = 0; i < vRange / 500; i++) {
    d.drawFastHLine(invertedX - 5, invertedY - (i * 500 * yScale), 5, WHITE);
  }
  
  for (int i = 0; i < dataLength - 1; i++) {
    float startPosX = invertedGraphX - (i * xScale);
    float startPosY = invertedGraphY - ((data[i] - minValue) * yScale);
    float endPosX = invertedGraphX - ((i + 1) * xScale);
    float endPosY = invertedGraphY - ((data[i + 1] - minValue) * yScale);
    
    d.drawLine(startPosX, startPosY, endPosX, endPosY, WHITE);
  }
  d.display();
  d.clearDisplay();
}
