#ifndef BUTTON_H
#define BUTTON_H

#include "Adafruit_TinyUSB.h"

int pressedBtnCount = 0;
bool timerIsEnabled = false;
long startMillis = 0;
bool checkTimerIsEnabled(int waitSeconds) {

  long waitMillis = waitSeconds * 1000;

  float current = millis();

  if(!timerIsEnabled){
    timerIsEnabled = true;
    startMillis = current;
  }

  float elapsedMillis = current - startMillis;
  float leftMillis = waitMillis - elapsedMillis;
  float leftSeconds = leftMillis / 1000;
  

  if(leftSeconds < 0){
    timerIsEnabled = false;
  }
  return timerIsEnabled;
}

bool pressAndCheckBtnPressedXTimesWithinYSedonds(int x, int y){
  pressedBtnCount = pressedBtnCount + 1;
  bool b = false;
  if(checkTimerIsEnabled(x)){
    if(pressedBtnCount >= y){
      timerIsEnabled = false;
      pressedBtnCount = 0;
      b = true;
    }
  }else{
    pressedBtnCount = 0;
  }
  return b;
}


#endif //BUTTON_H