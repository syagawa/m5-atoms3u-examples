#include "USBHIDKeyboard.h"
#include "json.h"
#include "led.h"

USBHIDKeyboard Keyboard;
String keyStr = "";
int existsKeyStr = 0;
JsonArray keyArray;
int existsKeyArray = 0;
int arraySize = 0;
int arraySizeMax = 10;
int keyIndex = 0;
int waitNextSeconds = 5;
String ledColor = "red";

DynamicJsonDocument settingsDoc(512);
String fName = "/ATOMS3U/SETTINGS.TXT";


bool waitNext = false;
long startMillisForWaitNext = 0;

int brightness = 100;

void keyboardWrite(String s){
  const char* str = s.c_str();
  uint8_t * buf = reinterpret_cast<uint8_t*>(const_cast<char*>(str));
  size_t len = strlen(str);
  Keyboard.write(buf, len);
}

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

void settingsApp(){

  settingsDoc = getJsonDocumentFromFile(fName);


  if(settingsDoc.containsKey("key")){
    keyStr = settingsDoc["key"].as<String>();
    existsKeyStr = 1;
  }else if(settingsDoc.containsKey("keys")){
    keyArray = settingsDoc["keys"];
    arraySize = keyArray.size();
    if(arraySize > arraySizeMax){
      arraySize = arraySizeMax;
    }
  }

  if(settingsDoc.containsKey("color")){
    ledColor = settingsDoc["color"].as<String>();
  }

  if(settingsDoc.containsKey("waitSeconds")){
    waitNextSeconds = settingsDoc["waitSeconds"].as<int>();
  }
}

void settingsApp_(DynamicJsonDocument doc){


  if(doc.containsKey("key")){
    keyStr = doc["key"].as<String>();
    existsKeyStr = 1;
  }else if(doc.containsKey("keys")){
    keyArray = doc["keys"];
    arraySize = keyArray.size();
    if(arraySize > arraySizeMax){
      arraySize = arraySizeMax;
    }
  }

  if(doc.containsKey("color")){
    ledColor = doc["color"].as<String>();
  }

  if(doc.containsKey("waitSeconds")){
    waitNextSeconds = doc["waitSeconds"].as<int>();
  }
}

void loopApp(bool pressed){
  
  if (pressed) {

    offLed();
    delay(10);
    liteLed(ledColor, brightness);
    if(existsKeyStr == 1){
      keyboardWrite(keyStr);
      stopWaitNext();
    }else if(arraySize > 0){
      String s = keyArray[keyIndex];
      keyboardWrite(s);
      keyIndex = keyIndex + 1;
      if(keyIndex >= arraySize){
        keyIndex = 0;
        stopWaitNext();
      }else{
        startWaitNext();
      }
    }
  }

  waitNext = checkWaitNextIsEnabled(waitNextSeconds);

  if(!waitNext){
    delay(10);
    offLed();
    keyIndex = 0;
  }

}