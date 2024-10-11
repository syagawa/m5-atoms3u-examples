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
String keyboardLayout = "";

#define KEY_LEFT_CTRL   0x80
#define KEY_LEFT_SHIFT  0x81
#define KEY_LEFT_ALT    0x82
#define KEY_LEFT_GUI    0x83
#define KEY_RIGHT_CTRL  0x84
#define KEY_RIGHT_SHIFT 0x85
#define KEY_RIGHT_ALT   0x86
#define KEY_RIGHT_GUI   0x87

#define KEY_UP_ARROW     0xDA
#define KEY_DOWN_ARROW   0xD9
#define KEY_LEFT_ARROW   0xD8
#define KEY_RIGHT_ARROW  0xD7
#define KEY_MENU         0xFE
#define KEY_SPACE        0x20
#define KEY_BACKSPACE    0xB2
#define KEY_TAB          0xB3
#define KEY_RETURN       0xB0
#define KEY_ESC          0xB1
#define KEY_INSERT       0xD1
#define KEY_DELETE       0xD4
#define KEY_PAGE_UP      0xD3
#define KEY_PAGE_DOWN    0xD6
#define KEY_HOME         0xD2
#define KEY_END          0xD5
#define KEY_NUM_LOCK     0xDB
#define KEY_CAPS_LOCK    0xC1
#define KEY_F1           0xC2
#define KEY_F2           0xC3
#define KEY_F3           0xC4
#define KEY_F4           0xC5
#define KEY_F5           0xC6
#define KEY_F6           0xC7
#define KEY_F7           0xC8
#define KEY_F8           0xC9
#define KEY_F9           0xCA
#define KEY_F10          0xCB
#define KEY_F11          0xCC
#define KEY_F12          0xCD
#define KEY_F13          0xF0
#define KEY_F14          0xF1
#define KEY_F15          0xF2
#define KEY_F16          0xF3
#define KEY_F17          0xF4
#define KEY_F18          0xF5
#define KEY_F19          0xF6
#define KEY_F20          0xF7
#define KEY_F21          0xF8
#define KEY_F22          0xF9
#define KEY_F23          0xFA
#define KEY_F24          0xFB
#define KEY_PRINT_SCREEN 0xCE
#define KEY_SCROLL_LOCK  0xCF
#define KEY_PAUSE        0xD0

#define LED_NUMLOCK    0x01
#define LED_CAPSLOCK   0x02
#define LED_SCROLLLOCK 0x04
#define LED_COMPOSE    0x08
#define LED_KANA       0x10

DynamicJsonDocument settingsDoc(512);
String fName = "/ATOMS3U/SETTINGS.TXT";


bool waitNext = false;
long startMillisForWaitNext = 0;

int brightness = 100;

String colors[] = {"BLUE","RED","GREEN","MAGENTA","YELLOW","PINK","BROWN","SKYBLUE","PURPLE"};

bool isLongPressedMode = false;
long startMillisInLongPressedMode = 0;
long waitMillisForNextIndex = 1000 * 4;
long startMillisInApp = 0;
bool isLongPressedEnable = false;
bool isLongPressed(int waitSeconds) {
  long waitMillis = waitSeconds * 1000;
  float current = millis();

  if(!isLongPressedEnable){
    isLongPressedEnable = true;
    startMillisInApp = current;
  }
  

  float elapsedMillis = current - startMillisInApp;
  float leftMillis = waitMillis - elapsedMillis;
  float leftSeconds = leftMillis / 1000;

  if(leftSeconds < 0){
    isLongPressedEnable = false;
    return true;
  }
  return false;
}


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
  if(s == "KEY_LEFT_CTRL"){
    Keyboard.press(KEY_LEFT_CTRL);
  }else if(s == "KEY_LEFT_SHIFT"){
    Keyboard.press(KEY_LEFT_SHIFT);
  }else if(s == "KEY_LEFT_ALT"){
    Keyboard.press(KEY_LEFT_ALT);
  }else if(s == "KEY_LEFT_GUI"){
    Keyboard.press(KEY_LEFT_GUI);
  }else if(s == "KEY_RIGHT_CTRL"){
    Keyboard.press(KEY_RIGHT_CTRL);
  }else if(s == "KEY_RIGHT_SHIFT"){
    Keyboard.press(KEY_RIGHT_SHIFT);
  }else if(s == "KEY_RIGHT_ALT"){
    Keyboard.press(KEY_RIGHT_ALT);
  }else if(s == "KEY_RIGHT_GUI"){
    Keyboard.press(KEY_RIGHT_GUI);
  }else if(s == "KEY_UP_ARROW"){
    Keyboard.press(KEY_UP_ARROW);
  }else if(s == "KEY_DOWN_ARROW"){
    Keyboard.press(KEY_DOWN_ARROW);
  }else if(s == "KEY_LEFT_ARROW"){
    Keyboard.press(KEY_LEFT_ARROW);
  }else if(s == "KEY_RIGHT_ARROW"){
    Keyboard.press(KEY_RIGHT_ARROW);
  }else if(s == "KEY_MENU"){
    Keyboard.press(KEY_MENU);
  }else if(s == "KEY_SPACE"){
    Keyboard.press(KEY_SPACE);
  }else if(s == "KEY_BACKSPACE"){
    Keyboard.press(KEY_BACKSPACE);
  }else if(s == "KEY_TAB"){
    Keyboard.press(KEY_TAB);
  }else if(s == "KEY_RETURN"){
    Keyboard.press(KEY_RETURN);
  }else if(s == "KEY_ESC"){
    Keyboard.press(KEY_ESC);
  }else if(s == "KEY_INSERT"){
    Keyboard.press(KEY_INSERT);
  }else if(s == "KEY_DELETE"){
    Keyboard.press(KEY_DELETE);
  }else if(s == "KEY_PAGE_UP"){
    Keyboard.press(KEY_PAGE_UP);
  }else if(s == "KEY_PAGE_DOWN"){
    Keyboard.press(KEY_PAGE_DOWN);
  }else if(s == "KEY_HOME"){
    Keyboard.press(KEY_HOME);
  }else if(s == "KEY_END"){
    Keyboard.press(KEY_END);
  }else if(s == "KEY_NUM_LOCK"){
    Keyboard.press(KEY_NUM_LOCK);
  }else if(s == "KEY_CAPS_LOCK"){
    Keyboard.press(KEY_CAPS_LOCK);
  }else if(s == "KEY_F1"){
    Keyboard.press(KEY_F1);
  }else if(s == "KEY_F2"){
    Keyboard.press(KEY_F2);
  }else if(s == "KEY_F3"){
    Keyboard.press(KEY_F3);
  }else if(s == "KEY_F4"){
    Keyboard.press(KEY_F4);
  }else if(s == "KEY_F5"){
    Keyboard.press(KEY_F5);
  }else if(s == "KEY_F6"){
    Keyboard.press(KEY_F6);
  }else if(s == "KEY_F7"){
    Keyboard.press(KEY_F7);
  }else if(s == "KEY_F8"){
    Keyboard.press(KEY_F8);
  }else if(s == "KEY_F9"){
    Keyboard.press(KEY_F9);
  }else if(s == "KEY_F10"){
    Keyboard.press(KEY_F10);
  }else if(s == "KEY_F11"){
    Keyboard.press(KEY_F11);
  }else if(s == "KEY_F12"){
    Keyboard.press(KEY_F12);
  }else if(s == "KEY_F13"){
    Keyboard.press(KEY_F13);
  }else if(s == "KEY_F14"){
    Keyboard.press(KEY_F14);
  }else if(s == "KEY_F15"){
    Keyboard.press(KEY_F15);
  }else if(s == "KEY_F16"){
    Keyboard.press(KEY_F16);
  }else if(s == "KEY_F17"){
    Keyboard.press(KEY_F17);
  }else if(s == "KEY_F18"){
    Keyboard.press(KEY_F18);
  }else if(s == "KEY_F19"){
    Keyboard.press(KEY_F19);
  }else if(s == "KEY_F20"){
    Keyboard.press(KEY_F20);
  }else if(s == "KEY_F21"){
    Keyboard.press(KEY_F21);
  }else if(s == "KEY_F22"){
    Keyboard.press(KEY_F22);
  }else if(s == "KEY_F23"){
    Keyboard.press(KEY_F23);
  }else if(s == "KEY_F24"){
    Keyboard.press(KEY_F24);
  }else if(s == "KEY_PRINT_SCREEN"){
    Keyboard.press(KEY_PRINT_SCREEN);
  }else if(s == "KEY_SCROLL_LOCK"){
    Keyboard.press(KEY_SCROLL_LOCK);
  }else if(s == "KEY_PAUSE"){
    Keyboard.press(KEY_PAUSE);
  }else if(s == "LED_NUMLOCK"){
    Keyboard.press(LED_NUMLOCK);
  }else if(s == "LED_CAPSLOCK"){
    Keyboard.press(LED_CAPSLOCK);
  }else if(s == "LED_SCROLLLOCK"){
    Keyboard.press(LED_SCROLLLOCK);
  }else if(s == "LED_COMPOSE"){
    Keyboard.press(LED_COMPOSE);
  }else if(s == "LED_KANA"){
    Keyboard.press(LED_KANA);
  }else{
    char firstChar = s.charAt(0);
    uint8_t firstCharAsUint8 = (uint8_t)firstChar; 
    Keyboard.press(firstCharAsUint8);
  }
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

  if(settingsDoc.containsKey("keyboardLayout")){
    keyboardLayout = settingsDoc["keyboardLayout"].as<String>();
  }


}

void loopApp(){

  // if(M5.BtnA.wasSingleClicked()){
  //   liteLed("yellow", brightness);
  // }else if(M5.BtnA.wasDoubleClicked()){
  //   liteLed("red", brightness);
  // }else if(M5.BtnA.isHolding()){
  //   liteLed("green", brightness);
  // }


  // return;

  if(M5.BtnA.isHolding()){
    liteLed("purple", brightness);
    if(!isLongPressedMode && isLongPressed(3)){
      liteLed("yellow", brightness);
      isLongPressedMode = true;
      startMillisInLongPressedMode = millis();
      delay(1000);
    }else if(isLongPressedMode){
      long passedMillis = startMillisInLongPressedMode - millis();
      if(waitMillisForNextIndex < passedMillis){
        keyIndex = keyIndex + 1;
        if(keyIndex >= arraySize){
          keyIndex = 0;
          isLongPressedMode = false;
          startMillisInLongPressedMode = millis();
        }else{
          String s = keyArray[keyIndex];
          liteLed(colors[keyIndex], brightness);
          keyboardPressSerial(s);
        }
      }
    }
    
  }else if (M5.BtnA.wasSingleClicked()) {
    isLongPressedMode = false;
    offLed();
    delay(10);
    if(existsKeyStr == 1){
      liteLed(ledColor, brightness);
      keyboardWrite(keyStr);
      stopWaitNext();
    }else if(arraySize > 0){
      String s = keyArray[keyIndex];
      liteLed(colors[keyIndex], brightness);
      // sendKeyboard(s);
      keyboardPressSerial(s);
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