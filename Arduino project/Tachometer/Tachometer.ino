#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <FreqMeasure.h>

#define DEBUG                 false
#define SCREEN_WIDTH          128
#define SCREEN_HEIGHT         64
#define BUTTON_PIN             2
#define FREQ_PIN              8
#define OLED_RESET            A4
#define SPLASH_TIME           1000
#define RPM_UPDATE_TIME       250
#define RPM_STACK_SIZE        50
#define AVG_MEASUREMENTS_LONG 24

// Variables
bool buttonState;                   // current state of the button
bool lastButtonState;               // previouse state of the button

unsigned long freqSum;              // Sum of all frequency measurements
byte freqCount;                     // total number of frequency measurements
unsigned long lastRPMUpdateTime;    // timestamp of the last RPM update function run
unsigned int lastRPM;               // last detected RPM updated in `updateRPMData`
unsigned int highestRPM;            // highest detected RPM updated in `updateRPMData`
int shortRPMStack[RPM_STACK_SIZE];  // short measurements in an array where all elements are pushed back everytime
int longRPMStack[RPM_STACK_SIZE];   // long measurements in an array where all avg of shortRPMStack are pushed to.
byte shortRPMStackCount;            // count of shoft measurements added to the array.

bool splashShown;                   // Bool splash shown
byte viewState = 0;                 // view state [0 = splash, 1 = rpm, 2 = shortGraph, 3 = longGraph]

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char volvoLogo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 
  0x3f, 0xff, 0xc3, 0xff, 0x87, 0xff, 0x80, 0xff, 0xfc, 0x3f, 0xff, 0xc3, 0xff, 0x0f, 0xff, 0xc0, 
  0x03, 0xfc, 0x00, 0x78, 0x1f, 0x87, 0xe0, 0x1f, 0xc0, 0x03, 0xfc, 0x00, 0x78, 0x3f, 0x07, 0xe0, 
  0x01, 0xfc, 0x00, 0x70, 0x3f, 0x03, 0xf0, 0x1f, 0xc0, 0x03, 0xfc, 0x00, 0xf0, 0x7e, 0x03, 0xf0, 
  0x01, 0xfe, 0x00, 0xf0, 0x7e, 0x03, 0xf8, 0x1f, 0xc0, 0x01, 0xfe, 0x00, 0xe0, 0x7e, 0x03, 0xf8, 
  0x00, 0xfe, 0x01, 0xe0, 0xfe, 0x01, 0xf8, 0x1f, 0xc0, 0x01, 0xfe, 0x01, 0xe0, 0xfc, 0x01, 0xf8, 
  0x00, 0xff, 0x01, 0xe0, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0xff, 0x01, 0xc0, 0xfc, 0x01, 0xfc, 
  0x00, 0x7f, 0x83, 0xc0, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0xff, 0x03, 0xc0, 0xfc, 0x01, 0xfc, 
  0x00, 0x7f, 0x83, 0xc0, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0x7f, 0x83, 0x80, 0xfc, 0x01, 0xfc, 
  0x00, 0x3f, 0xc7, 0x80, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x18, 0x3f, 0x87, 0x80, 0xfc, 0x01, 0xfc, 
  0x00, 0x3f, 0xc7, 0x00, 0xfe, 0x01, 0xfc, 0x1f, 0xc0, 0x18, 0x3f, 0xc7, 0x00, 0xfc, 0x01, 0xf8, 
  0x00, 0x1f, 0xef, 0x00, 0x7e, 0x03, 0xf8, 0x1f, 0xc0, 0x18, 0x1f, 0xcf, 0x00, 0x7e, 0x03, 0xf8, 
  0x00, 0x1f, 0xfe, 0x00, 0x7f, 0x03, 0xf0, 0x1f, 0xc0, 0x38, 0x1f, 0xfe, 0x00, 0x7e, 0x03, 0xf0, 
  0x00, 0x0f, 0xfe, 0x00, 0x3f, 0x87, 0xe0, 0x1f, 0xc0, 0xf8, 0x0f, 0xfe, 0x00, 0x3f, 0x07, 0xe0, 
  0x00, 0x0f, 0xfc, 0x00, 0x0f, 0xff, 0xc1, 0xff, 0xff, 0xf8, 0x0f, 0xfc, 0x00, 0x0f, 0xff, 0xc0, 
  0x00, 0x07, 0xfc, 0x00, 0x03, 0xfe, 0x00, 0xff, 0xff, 0xf8, 0x07, 0xfc, 0x00, 0x03, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  //Serial.begin(9600);

  setupDisplay();
  setupMeasureRPM();
  setupButton();
}

void loop() {
  updateSplash();

  measureRPM();
  updateRPMData();
  
  buttonAction();
  
  viewLoop();
}


// RPM

void setupMeasureRPM() {
  pinMode(FREQ_PIN, INPUT);
  FreqMeasure.begin();
}

void measureRPM() {
  if (DEBUG) { // Fill with fake data
    freqSum += random(256816, 524549);
    freqCount++;
  } else {  
    if (FreqMeasure.available()) {
      freqSum += FreqMeasure.read();
      freqCount++;
    }
  }
}

void updateRPMData() {
  if ((millis() - lastRPMUpdateTime) >= RPM_UPDATE_TIME) {
    lastRPMUpdateTime += RPM_UPDATE_TIME;
    //Serial.println(freqCount); // Log number of measurements.
    lastRPM = (FreqMeasure.countToFrequency(freqSum / freqCount) * 60) / 2; // 4 stroke engine, 2 sparks per rotation.
    if (highestRPM < lastRPM) {
      highestRPM = lastRPM;
    }

    // Add measurements to secStack
    pushToStack(shortRPMStack, lastRPM);
    shortRPMStackCount++;
    // Get avg of stack and move them to minStack.
    if (shortRPMStackCount >= AVG_MEASUREMENTS_LONG) {
      shortRPMStackCount = 0;
      unsigned long sum = 0;
      Serial.println("--");
      for (byte i = 0; i < AVG_MEASUREMENTS_LONG; i++) {
          Serial.println(shortRPMStack[i]);
          sum += shortRPMStack[i];
      }
      
      pushToStack(longRPMStack, sum / AVG_MEASUREMENTS_LONG);
    }
    
    freqCount = 0;
    freqSum = 0;
  }
}


// Display

void setupDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
}

void viewLoop() {
  if (viewState == 0) {
    splashView();
  } else if (viewState == 1) {
    rpmView();
  } else if (viewState == 2) {
    shortGraphView();
  } else if (viewState == 3) {
    longGraphView();
  } else {
    statsView();
  }
}

void splashView() {
  display.clearDisplay();
  display.drawBitmap(0, 23,volvoLogo, 128, 18, WHITE);
  display.display();
}

void rpmView() {
  display.clearDisplay();
  display.setTextSize(5);
  byte leftPadding = 5;
  if (lastRPM < 1000) {
    leftPadding = 35;
  }
  display.setCursor(leftPadding, 15);
  display.println(lastRPM);
  display.display();
}

void shortGraphView() {
  unsigned int dataLength = sizeof(shortRPMStack) / sizeof(shortRPMStack[0]);

  unsigned int minValue = 0;
  unsigned int maxValue = 0;
  for (int i = 0; i < dataLength; i++) {
    int value = shortRPMStack[i];
    if (minValue > value || minValue == 0) {
      minValue = value;
    }
    if (maxValue < value) {
      maxValue = value;
    }
  }
  
  minValue = (int)(floor((float)minValue / 1000) * 1000);
  maxValue = (int)(ceil((float)maxValue / 1000) * 1000);
  
  drawScrollingGraph(display, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, shortRPMStack, dataLength, minValue, maxValue);
}

void longGraphView() {
  unsigned int dataLength = sizeof(longRPMStack) / sizeof(longRPMStack[0]);

  unsigned int minValue = 0;
  unsigned int maxValue = 0;
  for (int i = 0; i < dataLength; i++) {
    int value = longRPMStack[i];
    if (minValue > value || minValue == 0) {
      minValue = value;
    }
    if (maxValue < value) {
      maxValue = value;
    }
  }

  minValue = (int)(floor((float)minValue / 1000) * 1000);
  maxValue = (int)(ceil((float)maxValue / 1000) * 1000);
  
  drawScrollingGraph(display, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, longRPMStack, dataLength, minValue, maxValue);
}

void statsView() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("High RPM: ");
  display.setTextSize(4);
  display.println(highestRPM);
  display.display();
}

//void debugView() {
//  display.clearDisplay();
//  if (buttonState) {
//    display.fillRect(0, 0, 128, 64, WHITE);
//  }
//  display.display();
//}

void updateSplash() {
  // Hide splash screen after SPLASH_TIME
  if (!splashShown && millis() > SPLASH_TIME) {
    splashShown = true;
    viewState += 1;
  }
}


// Button

void setupButton() {
  pinMode(BUTTON_PIN, INPUT);
}

void buttonAction() {
  buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == buttonState) {
    return;
  }
  delay(20);
  buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState != buttonState) {
    lastButtonState = buttonState;

    // Change viewState when button pushed.
    if (buttonState == HIGH) {
      viewState++;
      if (viewState == 5) {
        viewState = 0;
      }
    }
  }
}


/*
Tachometer
4 screens:
- Launchscreen (Volvo logo)
- 4 digit number
- Graph (every half second)
- Graph (last 5min)


Code
Vars
- bool lastButtonState
- unsigned int freqSum
- byte freqCount
- bool splashShown
- byte display
- Stack Stack1
- Stack Stack2
- byte Stack1 counter
- Bool Redraw1
- Bool Redraw2

Loop 4main actions
- buttonAction (digital read)
  - changeDetection (detected, update display byte)
- Frequency measure (runs all the time, store sum + counter)
- updateMeasurements (runs every half second calculate avg. frequency on sum + count, clear)
    - redraw1 bool
    - pushToStack
    - IncrementCounter
    - every 60 measurements calc avg pushToSecondStack.
    - Redraw2 bool
- Update screen
  - update 4 digit number (check redraw1 bool, set to false)
  - update graph half second (check redraw1 bool, set to false)
  - update graph 30 seconds (check redraw2 bool. Set to false)

GraphLib
- drawScrollingGraph
StackLib
- pushToStack
*/
