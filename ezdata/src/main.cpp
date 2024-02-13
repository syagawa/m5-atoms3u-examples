
#include <M5Unified.h>
#include "USB.h"
#include <EspEasyLED.h>

#include "settings_ezdata.h"
#include "ezdata.h"

EspEasyLED *rgbled;

void offLed(){
  rgbled->setBrightness(0);
  rgbled->show();
}

void onLed(){
  rgbled->setBrightness(100);
  rgbled->show();
}


void flickLed(int num, String c){

  onLed();
  for(int i = 0; i < num; i++){
    if(c == "red"){
      rgbled->showColor(255, 0, 0);//red
    }else if(c == "yellow"){
      rgbled->showColor(255, 255, 0);//yellow
    }else if(c == "magenta"){
      rgbled->showColor(255, 0, 225);//magenta
    }else if(c == "lime"){
      rgbled->showColor(0, 255, 0);//lime
    }else if(c == "blue"){
      rgbled->showColor(0, 0, 225);//blue
    }else if(c == "cyan"){
      rgbled->showColor(0, 255, 225);//cyan
    }else{
      rgbled->showColor(0, 0, 0);//black
    }
    delay(20);
    rgbled->showColor(255, 255, 255);//white
    delay(20);
  }
  offLed();

}

void setup(){
  auto cfg = M5.config();
  M5.begin(cfg);

  rgbled = new EspEasyLED(GPIO_NUM_35, 1, 20);

  setupEzData(ezdata_ssid, ezdata_password, ezdata_token);

  flickLed(5, "cyan");
}


void loop(){
  M5.update();
  if(M5.BtnA.wasPressed()){
    flickLed(2, "lime");
    ezAddToList("testkey", millis());
}
