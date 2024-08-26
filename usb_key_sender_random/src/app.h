#include "USBHIDKeyboard.h"
#include "json.h"
#include "led.h"
#include "UUID.h"

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

String randomMode = "uuid";


DynamicJsonDocument settingsDoc(512);
String fName = "/ATOMS3U/SETTINGS.TXT";


bool waitNext = false;
long startMillisForWaitNext = 0;

int brightness = 100;

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

  String str = parts1[0];

  if(str == "press"){
    String parts2[4];
    char delimiter2 = ',';
    splitString(parts1[1], delimiter2, parts2, 4);
    String s1 = parts2[0];
    String s2 = parts2[1];
    String s3 = parts2[2];
    String s4 = parts2[3];

    if(s1.length() > 0){
      keyboardPress(s1);
    }
    delay(50);
    if(s2.length() > 0){
      keyboardPress(s2);
    }
    delay(50);
    if(s3.length() > 0){
      keyboardPress(s3);
    }
    delay(50);
    if(s4.length() > 0){
      keyboardPress(s4);
    }
    delay(50);
    keyboardReleaseAll();

  }else if(str == "release"){
    Keyboard.releaseAll();
  }else if(str == "open"){
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);
    Keyboard.releaseAll();
    const char* str = parts1[1].c_str();
    uint8_t * buf = reinterpret_cast<uint8_t*>(const_cast<char*>(str));
    size_t len = strlen(str);
    Keyboard.write(buf, len);
    delay(100);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.release(KEY_RETURN);
  }else{
    const char* str = s.c_str();
    uint8_t * buf = reinterpret_cast<uint8_t*>(const_cast<char*>(str));
    size_t len = strlen(str);
    Keyboard.write(buf, len);
  }

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

UUID uuid;

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

}

void loopApp(bool pressed, bool longpressed){

  if(longpressed){
    liteLed("green", brightness);
  }else if (pressed) {



    offLed();
    delay(10);
    liteLed(ledColor, brightness);

    if(randomMode == "uuid"){
      uint8_t a14 = analogRead(14);
      // randomSeed(a14);
      uuid.seed(a14);
      uuid.generate();

      String s_uuid =  String(uuid.toCharArray());
      keyboardWrite("\nuuid\:\s");
      keyboardWrite(s_uuid);


    }else{
      uint8_t a14 = analogRead(14);
      randomSeed(a14);
      String s_a14 = String(a14);
      int random14 = random(0, 100);
      String s_random14 = String(random14);
      // keyboardWrite("\na14\n");
      // keyboardWrite(s_a14);
      keyboardWrite("\nrandom14\n");
      keyboardWrite(s_random14);


      uint8_t a17 = analogRead(17);
      randomSeed(a17);
      String s_a17 = String(a17);
      int random17 = random(0, 100);
      String s_random17 = String(random17);

      keyboardWrite("\nrandom17\n");
      keyboardWrite(s_random17);

    }



    keyboardWrite("\n\n");



  }

  waitNext = checkWaitNextIsEnabled(waitNextSeconds);

  if(!waitNext){
    delay(10);
    offLed();
    keyIndex = 0;
  }

}