#include <EspEasyLED.h>
EspEasyLED *rgbled;

void initLed(){
  rgbled = new EspEasyLED(GPIO_NUM_35, 1, 20);
}

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
    delay(100);
    rgbled->showColor(255, 255, 255);//white
    delay(100);
  }
  offLed();

}