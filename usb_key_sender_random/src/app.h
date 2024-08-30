#include "USBHIDKeyboard.h"
#include "json.h"
#include "led.h"
#include "UUID.h"

USBHIDKeyboard Keyboard;
UUID uuid;
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

#define KEY_LEFT_SHIFT  0x81

bool waitNext = false;
long startMillisForWaitNext = 0;
int brightness = 100;

String randomMode = "";
long randomMin = 0;
long randomMax = 100;
int seedPort = 14; //14,17
String prefix = "";
String suffix = "";

int splitString(String data, char delimiter, String parts[], int maxParts) {
  int partCount = 0;
  int startIndex = 0;
  int endIndex = 0;

  while ((endIndex = data.indexOf(delimiter, startIndex)) >= 0) {
    if (partCount < maxParts) {
      parts[partCount++] = data.substring(startIndex, endIndex);
      startIndex = endIndex + 1;
    } else {
      break;
    }
  }

  // 最後の部分を追加します
  if (startIndex < data.length() && partCount < maxParts) {
    parts[partCount++] = data.substring(startIndex);
  }

  return partCount;
}

void keyboardWrite(String s){
  const char* str = s.c_str();
  uint8_t * buf = reinterpret_cast<uint8_t*>(const_cast<char*>(str));
  size_t len = strlen(str);
  Keyboard.write(buf, len);
}

void keyboardPressSerial(String s){
  int length = s.length();
  for(int i = 0; i < length; i++){
    char c = s.charAt(i);
    if(c == ':'){
      // Keyboard.press(KEY_LEFT_SHIFT);
      // int hexValue1 = 0x3A;
      // char colon1 = (char)hexValue1;
      // int hexValue2 = 0x2a;
      // char colon2 = (char)hexValue2;
      // int hexValue3 = 0x33;
      // char colon3 = (char)hexValue3;

      // Keyboard.press(KEY_LEFT_SHIFT);
      // Keyboard.pressRaw(hexValue1);

      // Keyboard.pressRaw(hexValue2);
      // Keyboard.pressRaw(hexValue3);

      // Keyboard.press(colon1);
      // Keyboard.press('a');
      // Keyboard.press(colon2);

      // Keyboard.pressRaw(colon1);
      // Keyboard.pressRaw(colon2);
      // Keyboard.press(0x33);
      // Keyboard.pressRaw(0x33);

    }else{
      uint8_t firstCharAsUint8 = (uint8_t)c;
      Keyboard.press(firstCharAsUint8);
    }
    // Keyboard.pressRaw(i + 58);
    Keyboard.releaseAll();
  }
}

void keyboardPress(String s){
  char firstChar = s.charAt(0);
  uint8_t firstCharAsUint8 = (uint8_t)firstChar;
  Keyboard.press(firstCharAsUint8);
  
}
void keyboardReleaseAll(){
  Keyboard.releaseAll();
}

void sendKeyboard(String s){
  char delimiter1 = ':';
  String parts1[2];
  splitString(s, delimiter1, parts1, 4);
  const char* str2 = s.c_str();
  uint8_t * buf = reinterpret_cast<uint8_t*>(const_cast<char*>(str2));
  size_t len = strlen(str2);
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

  if(settingsDoc.containsKey("randomMode")){
    randomMode = settingsDoc["randomMode"].as<String>();
  }
  
  if(settingsDoc.containsKey("randomMin")){
    randomMin = settingsDoc["randomMin"].as<long>();
  }

  if(settingsDoc.containsKey("randomMax")){
    randomMax = settingsDoc["randomMax"].as<long>();
  }

  if(settingsDoc.containsKey("seedPort")){
    seedPort = settingsDoc["seedPort"].as<int>();
  }

  if(settingsDoc.containsKey("prefix")){
    prefix = settingsDoc["prefix"].as<String>();
  }

  if(settingsDoc.containsKey("suffix")){
    suffix = settingsDoc["suffix"].as<String>();
  }

}



void loopApp(bool pressed, bool longpressed){

  if(longpressed){
    liteLed("green", brightness);
  }else if (pressed) {



    offLed();
    delay(10);
    liteLed(ledColor, brightness);

    String write_s = "";

    if(randomMode == "uuid"){
      uint8_t a_read = analogRead(seedPort);
      uuid.seed(a_read);
      uuid.generate();

      write_s =  String(uuid.toCharArray());
      // keyboardWrite("\nuuid\:\s");
      // keyboardWrite(s_uuid);

    }else{
      uint8_t a_read = analogRead(seedPort);
      randomSeed(a_read);
      int rand = random(randomMin, randomMax);
      write_s = String(rand);
      // keyboardWrite(s_random);

    }

    if(write_s.length() > 0){

      Keyboard.releaseAll();
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.pressRaw(58);
      Keyboard.releaseAll();
      for(int test_i = 0; test_i < 20; test_i++){
        Keyboard.press(test_i + 58);
        Keyboard.releaseAll();
      }


      if(prefix.length() > 0){
        keyboardPressSerial(prefix);
      }

      keyboardWrite(write_s);

      if(suffix.length() > 0){
        keyboardPressSerial(suffix);
      }


      keyboardWrite("\n");
    }


  }

  waitNext = checkWaitNextIsEnabled(waitNextSeconds);

  if(!waitNext){
    delay(10);
    offLed();
    keyIndex = 0;
  }

}