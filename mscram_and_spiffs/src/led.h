#ifndef LED_H
#define LED_H

#include <Arduino.h>

void initLed();
void offLed();
void onLed();
void flickLed(int num, String c);
void changeColor(String c);
void liteLed(String c);


#endif //LED_H