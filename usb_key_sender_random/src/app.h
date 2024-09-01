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
    if(c == '*'){
      Keyboard.press(34);
    }else if(c == '\''){
      Keyboard.press(38);
    }else if(c ==':'){
      Keyboard.press(39);
    }else if(c ==')'){
      Keyboard.press(40);
    }else if(c =='('){
      Keyboard.press(42);
    }else if(c =='~'){
      Keyboard.press(43);
    }else if(c ==','){
      Keyboard.press(44);
    }else if(c =='+'){
      Keyboard.press(58);
    }else if(c =='^'){
      Keyboard.press(61);
    }else if(c == '"'){
      Keyboard.press(64);
    }else if(c == '@'){
      Keyboard.press(91);
    }else if(c == ']'){
      Keyboard.press(92);
    }else if(c == '['){
      Keyboard.press(93);
    }else if(c == '&'){
      Keyboard.press(94);
    }else if(c == '='){
      Keyboard.press(95);
    // }else if(c == '全/半'){
    //   Keyboard.press(96);
    }else if(c == '`'){
      Keyboard.press(123);
    }else if(c == '}'){
      Keyboard.press(124);
    }else if(c =='{'){
      Keyboard.press(125);
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

    }else{
      uint8_t a_read = analogRead(seedPort);
      randomSeed(a_read);
      int rand = random(randomMin, randomMax);
      write_s = String(rand);
      // keyboardWrite(s_random);

    }



// 32 space?      "0x2c           ' '"
// 33 !           "0x1e | SHIFT  !"
// 34 *           "0x34 | SHIFT  doublequote" ooo *
// 35 #           "0x20 | SHIFT  #"
// 36 $           "0x21 | SHIFT  $"
// 37 %           "0x22 | SHIFT  %"
// 38 '           "0x24 | SHIFT  &"           ooo '
// 39 :           "0x34          singlequote" ooo :
// 40 )           "0x26 | SHIFT  ("           ooo )
// 41 null?       "0x27 | SHIFT  )"
// 42 (           "0x25 | SHIFT  *"           ooo (
// 43 ~           "0x2e | SHIFT  +"           ooo ~
// 44 ,           "0x36          "            ooo ,
// 45 -           "0x2d          -"
// 46 .           "0x37          ."
// 47 /           "0x38          /"
// 58 +           "0x33 | SHIFT  :"           ooo +
// 59 ;           "0x33          ;"
// 60 <           "0x36 | SHIFT  <"
// 61 ^           "0x2e          ="           ooo ^
// 62 >           "0x37 | SHIFT  >"
// 63 ?           "0x38 | SHIFT  ?"
// 64 "           "0x1f | SHIFT  @"           ooo "
// 91 @            "0x2f          ["          ooo @
// 92 ]            "0x31          bslash"     ooo ]
// 93 [            "0x30          ]"          ooo [
// 94 &            "0x23 | SHIFT  ^"          ooo &
// 95 =            "0x2d | SHIFT  _"          ooo =
// 96 半/全        "0x35          `"          ooo 半/全
// 123 `           "0x2f | SHIFT  {"          ooo `
// 124 }           "0x31 | SHIFT  |"          ooo }
// 125 {           "0x30 | SHIFT  }"          ooo {
// 126             "0x35 | SHIFT  ~"
// 127             "0              DEL"



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

  waitNext = checkWaitNextIsEnabled(waitNextSeconds);

  if(!waitNext){
    delay(10);
    offLed();
    keyIndex = 0;
  }

}