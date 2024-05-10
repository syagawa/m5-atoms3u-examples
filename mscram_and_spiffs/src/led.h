#ifndef LED_H
#define LED_H

#include <Arduino.h>

#include <EspEasyLED.h>
EspEasyLED *rgbled;

void initLed(){
  rgbled = new EspEasyLED(GPIO_NUM_35, 1, 20);
}

void offLed(){
  rgbled->setBrightness(0);
  rgbled->show();
}

void onLed(int brightness = 100){
  rgbled->setBrightness(brightness);
  rgbled->show();
}

void changeColor(String c){
  if(c == "BLACK" || c == "black"){
    rgbled->showColor(0, 0, 0);
  }else if(c == "NAVY" || c == "navy"){
    rgbled->showColor(0, 0, 128);
  }else if(c == "DARKGREEN" || c == "darkgreen"){
    rgbled->showColor(0, 128, 0);
  }else if(c == "DARKCYAN" || c == "darkcyan"){
    rgbled->showColor(0, 128, 128);
  }else if(c == "MAROON" || c == "maroon"){
    rgbled->showColor(128, 0, 0);
  }else if(c == "PURPLE" || c == "purple"){
    rgbled->showColor(128, 0, 128);
  }else if(c == "OLIVE" || c == "olive"){
    rgbled->showColor(128, 128, 0);
  }else if(c == "LIGHTGREY" || c == "lightgrey"){
    rgbled->showColor(211, 211, 211);
  }else if(c == "LIGHTGRAY" || c == "lightgray"){
    rgbled->showColor(211, 211, 211);
  }else if(c == "DARKGREY" || c == "darkgrey"){
    rgbled->showColor(128, 128, 128);
  }else if(c == "DARKGRAY" || c == "darkgray"){
    rgbled->showColor(128, 128, 128);
  }else if(c == "BLUE" || c == "blue"){
    rgbled->showColor(0, 0, 255);
  }else if(c == "GREEN" || c == "green"){
    rgbled->showColor(0, 255, 0);
  }else if(c == "CYAN" || c == "cyan"){
    rgbled->showColor(0, 255, 255);
  }else if(c == "RED" || c == "red"){
    rgbled->showColor(255, 0, 0);
  }else if(c == "MAGENTA" || c == "magenta"){
    rgbled->showColor(255, 0, 255);
  }else if(c == "YELLOW" || c == "yellow"){
    rgbled->showColor(255, 255, 0);
  }else if(c == "WHITE" || c == "white"){
    rgbled->showColor(255, 255, 255);
  }else if(c == "ORANGE" || c == "orange"){
    rgbled->showColor(255, 180, 0);
  }else if(c == "GREENYELLOW" || c == "greenyellow"){
    rgbled->showColor(180, 255, 0);
  }else if(c == "PINK" || c == "pink"){
    rgbled->showColor(255, 192, 203);
  }else if(c == "BROWN" || c == "brown"){
    rgbled->showColor(150, 75, 0);
  }else if(c == "GOLD" || c == "gold"){
    rgbled->showColor(255, 215, 0);
  }else if(c == "SILVER" || c == "silver"){
    rgbled->showColor(192, 192, 192);
  }else if(c == "SKYBLUE" || c == "skyblue"){
    rgbled->showColor(135, 206, 235);
  }else if(c == "VIOLET" || c == "violet"){
    rgbled->showColor(180, 46, 226);
  }else{
    rgbled->showColor(255, 255, 255);
  }
}

void liteLed(String c, int brightness = 100){
  onLed(brightness);
  changeColor(c);
}

void flickLed(int num, String c){
  onLed();
  for(int i = 0; i < num; i++){
    changeColor(c);
    delay(40);
    changeColor("white");
    delay(10);
  }
  offLed();
}

#endif //LED_H