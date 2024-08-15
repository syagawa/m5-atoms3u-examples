#include "json.h"
#include "led.h"


String fName = "/ATOMS3U/SETTINGS.TXT";


bool waitNext = false;
long startMillisForWaitNext = 0;

int brightness = 100;


void startWaitNext(){
  waitNext = true;
  float current = millis();
  startMillisForWaitNext = current;
}
void stopWaitNext(){
  waitNext = false;
}

bool checkWaitNextIsEnabled(int waitSeconds) {
  if(!waitNext){
    return false;
  }

  long waitMillis = waitSeconds * 1000;
  float current = millis();

  float elapsedMillis = current - startMillisForWaitNext;
  float leftMillis = waitMillis - elapsedMillis;
  float leftSeconds = leftMillis / 1000;

  if(leftSeconds < 0){
    return false;
  }
  return true;
}

JsonArray colorArray;
int arraySizeMax = 10;
int colorIndex = 0;
int arraySize = 0;
String colorMode = "";
String ledColor = "red";
int light = 0;
int offNext = 0;

void settingsApp(){

  settingsDocInMain = getJsonDocumentFromFile(fName);

  if(settingsDocInMain.containsKey("colors")){
    colorMode = "colors";
    colorArray = settingsDocInMain["colors"];
    arraySize = colorArray.size();
    if(arraySize > arraySizeMax){
      arraySize = arraySizeMax;
    }
  }else if(settingsDocInMain.containsKey("color")){
    colorMode = "color";
    ledColor = settingsDocInMain["color"].as<String>();
  }

}


void loopApp(bool pressed, bool longpressed){

  if (pressed) {

    if(colorMode == "color"){
      if(light == 0){
        liteLed(ledColor, brightness);
        light = 1;
      }else if(light == 1){
        offLed();
        light = 0;
      }
    }else if(colorMode == "colors"){
      if(offNext == 1){
        offLed();
        offNext = 0;
      }else if(offNext == 0){
        String c = colorArray[colorIndex];
        liteLed(c, brightness);
        colorIndex = colorIndex + 1;
        if(colorIndex >= arraySize){
          colorIndex = 0;
          offNext = 1;
        }
      }

    }
  }

}