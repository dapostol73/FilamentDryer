#include <Arduino.h>
#include "MaterialDefinition.h"
#include "ClimateControl.h"
#include "DisplayControl.h"

#define DEBUG true
#define MODE_WAIT 0
#define MODE_TEMP 1
#define MODE_TIME 2
#define MODE_RUN 3

const int selectButtonPin = 2;
const int plusButtonPin = 3;
const int minusButtonPin = 4;
int activeMode = 0;

MaterialDefinition matChoices[] = {
  MaterialDefinition { "PLA", 50, 240 },
  MaterialDefinition { "PETG", 65, 180 },
  MaterialDefinition { "TPU", 45, 300 },
  MaterialDefinition { "ABS", 60, 300 },
  MaterialDefinition { "Nylon", 70, 600 }
};

MaterialDefinition matChoice = matChoices[0];
int maxMatSelect = sizeof(matChoices)/sizeof(MaterialDefinition)-1;
int matSelect = 0;

int timeCounter = 0;
int timeMinutes = 0;
int timeSeconds = 0;

// Read temperature as Celsius (the default)
float heater = 0;
float temperature = 0;
float humidity = 0;
char heaterText[5];
char temperatureText[5];
char humidityText[5];
char hoursText[3];
char minutesText[3];
char secondsText[3];

char displayText[100];

void executeDebugInfo() {
  strcpy(displayText, "DBG - ");
  strcat(displayText, matChoice.Name);
  displayLineInfo(0, displayText, 1);
  
  if (climateMeasureEnvironment(&temperature, &humidity) == true) {
    dtostrf(temperature, 1, 0, temperatureText);
    dtostrf(humidity, 1, 0, humidityText);
    strcpy(displayText, temperatureText);
    strcat(displayText, "\xF7""C ");
    strcat(displayText, humidityText);
    strcat(displayText, "%");
    displayLineInfo(1, displayText, 0);
  }

  if (climateMeasureHeater(&heater) == true) {
    dtostrf(heater, 1, 0, heaterText);
    strcpy(displayText, heaterText);
    strcat(displayText, "\xF7""C ");
    displayLineInfo(2, displayText, 0);
  }
}

void executeWaitMode() {
  if (digitalRead(plusButtonPin) == LOW) {
    matSelect++;
    delay(250);
  }
  else if (digitalRead(minusButtonPin) == LOW) {
    matSelect--;
    delay(250);
  }

  if (matSelect > maxMatSelect) {
    matSelect = 0;
  }
  else if (matSelect < 0) {
    matSelect = maxMatSelect;
  }

  matChoice = matChoices[matSelect];
  int timeHrs = matChoice.Minutes/60;

  itoa(matChoice.Temp, temperatureText, 10);
  itoa(timeHrs, hoursText, 10);
  
  displayLineInfo(0, F("\xAE Type \x04 \xAF"), 1); 
  displayLineInfo(1, matChoice.Name, 0);
  strcpy(displayText, temperatureText);
  strcat(displayText, "\xF7""C ");
  strcat(displayText, hoursText);
  strcat(displayText, " HR");
  displayLineInfo(2, displayText, 0);
}

void executeTimeMode() {
  int timeChoice = 0;
  int timeChange = 60;
  if (digitalRead(plusButtonPin) == LOW) {
    timeChoice++;
    delay(250);
  }
  else if (digitalRead(minusButtonPin) == LOW) {
    timeChoice--;
    delay(250);
  }

  if (timeChoice > 0) {
    matChoice.Minutes += timeChange;
  }
  else if (timeChoice < 0) {
    matChoice.Minutes -= timeChange;
  }

  if (matChoice.Minutes < timeChange) {
    matChoice.Minutes = timeChange;
  }

  int timeHrs = matChoice.Minutes/60;
  itoa(timeHrs, hoursText, 10);
  
  displayLineInfo(0, F("\xAE Time \xE7 \xAF"), 1); 
  displayLineInfo(1, matChoice.Name, 0);
  strcpy(displayText, "Set: ");
  strcat(displayText, hoursText);
  strcat(displayText, " HR");  
  displayLineInfo(2, displayText, 0);  
}

void executeTempMode() {
  int tempChoice = 0;
  int tempChange = 5;
  int tempMin = 25;
  if (digitalRead(plusButtonPin) == LOW) {
    tempChoice++;
    delay(250);
  }
  else if (digitalRead(minusButtonPin) == LOW) {
    tempChoice--;
    delay(250);
  }

  if (tempChoice > 0) {
    matChoice.Temp += tempChange;
  }
  else if (tempChoice < 0) {
    matChoice.Temp -= tempChange;
  }

  if (matChoice.Temp < tempMin) {
    matChoice.Temp = tempMin;
  }

  itoa(matChoice.Temp, temperatureText, 10);
  
  displayLineInfo(0, F("\xAE Temp \xF7 \xAF"), 1); 
  displayLineInfo(1, matChoice.Name, 0);
  strcpy(displayText, "Set: ");
  strcat(displayText, temperatureText);
  strcat(displayText, "\xF7""C");
  displayLineInfo(2, displayText, 0);  
}

void executeRunMode() {
  
  if (DEBUG == true) {
    executeDebugInfo();
    return;
  }
  
  int timeStart = matChoice.Minutes * 60;
  int timeCounter = timeStart;
  int timeHours = 0;
  int timeMinutes = 0;
  int timeSeconds = 0;
  float percentage = 0;
    
  while(activeMode == MODE_RUN) {     
    // Kick off the system
    if (timeCounter == timeStart) {
      // *****************Start heater
    }

    // Updated the display info every 5 seconds
    if (climateMeasureEnvironment(&temperature, &humidity) == true) {
      dtostrf(temperature, 1, 0, temperatureText);
      dtostrf(humidity, 1, 0, humidityText);
      strcpy(displayText, matChoice.Name);
      strcat(displayText, " - ");
      strcat(displayText, temperatureText);
      strcat(displayText, "\xF7""C ");
      strcat(displayText, humidityText);
      strcat(displayText, "%");
      displayLineInfo(0, displayText, 0, true);
    }
    
    // Calculate minutes and seconds and convert to char
    timeHours = timeCounter/3600;
    timeMinutes = timeCounter/60%60;
    timeSeconds = timeCounter%60;

    dtostrf(matChoice.Temp, 2, 1, temperatureText);
    itoa(timeHours, hoursText, 10);
    itoa(timeMinutes, minutesText, 10);
    itoa(timeSeconds, secondsText, 10);
    
    // Handle updating the display time
    strcpy(displayText, "  ");
    strcat(displayText, hoursText);

    if (timeMinutes < 10) {
      strcat(displayText, ":0");
      strcat(displayText, minutesText);
    }
    else {
      strcat(displayText, ":");
      strcat(displayText, minutesText);
    }
    
    if (timeSeconds < 10) {
      strcat(displayText, ":0");
      strcat(displayText, secondsText);
    }
    else {
      strcat(displayText, ":");
      strcat(displayText, secondsText);
    }
    
    displayLineInfo(1, displayText, 0);
    displayLineInfo(2, F(" Cancel \xEF"), 1);
    // Update precentage amount
    percentage = (1.0 - (timeCounter*1.0/timeStart)) * 100.0;
    displayProgress(percentage);

    // Check if we are done, when timer is 0 or less    
    if(timeCounter < 1){
      activeMode = MODE_WAIT; // stop sequence
      displayLineInfo(1, F("Done!"), 1);
      displayClearLine(2);
    }

    // Check if the user canceled
    if (digitalRead(selectButtonPin) == LOW) {
      activeMode = MODE_WAIT; // stop sequence
      displayLineInfo(1, F("Canceled!"));
      displayClearLine(2);
    }
    
    // Handle time increment
    timeCounter--;
    delay(1000);
  }

  // Once we break the while loop, reset everything
  // *****************Turn off heater
  delay(2500);
  displayProgress(0);
}

void setup() {
  //Serial.begin(9600);
  displayInitialize();

  climateInitialize();

  pinMode(selectButtonPin, INPUT_PULLUP);
  pinMode(plusButtonPin, INPUT_PULLUP); 
  pinMode(minusButtonPin, INPUT_PULLUP);
  //Serial.println(F("System initialized"));

  displayLedColor(0, 0, 128, 2000);
  displayLedClear();
}

void loop() {
  if (digitalRead(selectButtonPin) == LOW && activeMode != MODE_RUN) {
    activeMode++;

    if (activeMode > MODE_RUN) {
      activeMode = 0;
    }
    
    delay(500);
  }
  
  switch (activeMode) {
    case MODE_WAIT:
      executeWaitMode();
      break;
    case MODE_TEMP:
      executeTempMode();
      break;
    case MODE_TIME:
      executeTimeMode();
      break;
    case MODE_RUN:
      executeDebugInfo();
      executeRunMode();
      break;
    default:
      display.clearDisplay();
      displayLineInfo(0, F("ERROR"), 1);
      displayLineInfo(1, F("Unknown Mode"));
      break;      
  }
}