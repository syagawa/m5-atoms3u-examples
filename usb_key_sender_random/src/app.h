#include "USBHIDKeyboard.h"
#include "json.h"
#include "led.h"
#include "UUID.h"

USBHIDKeyboard Keyboard;
UUID uuid;

String ledColor = "red";

DynamicJsonDocument settingsDoc(512);
String fName = "/ATOMS3U/SETTINGS.TXT";

#define KEY_LEFT_SHIFT  0x81

int brightness = 100;

String randomMode = "";
long randomMin = 0;
long randomMax = 100;
int seedPort = 14; //14,17
String prefix = "";
String suffix = "";
String keyboardLayout = "";

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
    uint8_t c_int = (uint8_t)c;
    if(keyboardLayout == "ja"){
      if(c == '*'){
        c_int = 34;
      }else if(c == '\''){
        c_int = 38;
      }else if(c ==':'){
        c_int = 39;
      }else if(c ==')'){
        c_int = 40;
      }else if(c =='('){
        c_int = 42;
      }else if(c =='~'){
        c_int = 43;
      }else if(c ==','){
        c_int = 44;
      }else if(c =='+'){
        c_int = 58;
      }else if(c =='^'){
        c_int = 61;
      }else if(c == '"'){
        c_int = 64;
      }else if(c == '@'){
        c_int = 91;
      }else if(c == ']'){
        c_int = 92;
      }else if(c == '['){
        c_int = 93;
      }else if(c == '&'){
        c_int = 94;
      }else if(c == '='){
        c_int = 95;
      // }else if(c == '全/半'){
      //   c_int = 96;
      }else if(c == '`'){
        c_int = 123;
      }else if(c == '}'){
        c_int = 124;
      }else if(c =='{'){
        c_int = 125;
      }
    }
    Keyboard.press(c_int);
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


void settingsApp(){

  settingsDoc = getJsonDocumentFromFile(fName);


  if(settingsDoc.containsKey("color")){
    ledColor = settingsDoc["color"].as<String>();
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

  if(settingsDoc.containsKey("keyboardLayout")){
    keyboardLayout = settingsDoc["keyboardLayout"].as<String>();
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

    }else{
      uint8_t a_read = analogRead(seedPort);
      randomSeed(a_read);
      int rand = random(randomMin, randomMax);
      write_s = String(rand);

    }


    if(write_s.length() > 0){

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

  delay(10);
  offLed();
  

}