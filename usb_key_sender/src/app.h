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


// #define KEY_LEFT_CTRL   0x80
// #define KEY_LEFT_SHIFT  0x81
// #define KEY_LEFT_ALT    0x82
// #define KEY_LEFT_GUI    0x83
// #define KEY_RIGHT_CTRL  0x84
// #define KEY_RIGHT_SHIFT 0x85
// #define KEY_RIGHT_ALT   0x86
// #define KEY_RIGHT_GUI   0x87

// #define KEY_UP_ARROW     0xDA
// #define KEY_DOWN_ARROW   0xD9
// #define KEY_LEFT_ARROW   0xD8
// #define KEY_RIGHT_ARROW  0xD7
#define KEY_MENU         0xFE
#define KEY_SPACE        0x20
// #define KEY_BACKSPACE    0xB2
// #define KEY_TAB          0xB3
// #define KEY_RETURN       0xB0
// #define KEY_ESC          0xB1
// #define KEY_INSERT       0xD1
// #define KEY_DELETE       0xD4
// #define KEY_PAGE_UP      0xD3
// #define KEY_PAGE_DOWN    0xD6
// #define KEY_HOME         0xD2
// #define KEY_END          0xD5
#define KEY_NUM_LOCK     0xDB
// #define KEY_CAPS_LOCK    0xC1
// #define KEY_F1           0xC2
// #define KEY_F2           0xC3
// #define KEY_F3           0xC4
// #define KEY_F4           0xC5
// #define KEY_F5           0xC6
// #define KEY_F6           0xC7
// #define KEY_F7           0xC8
// #define KEY_F8           0xC9
// #define KEY_F9           0xCA
// #define KEY_F10          0xCB
// #define KEY_F11          0xCC
// #define KEY_F12          0xCD
// #define KEY_F13          0xF0
// #define KEY_F14          0xF1
// #define KEY_F15          0xF2
// #define KEY_F16          0xF3
// #define KEY_F17          0xF4
// #define KEY_F18          0xF5
// #define KEY_F19          0xF6
// #define KEY_F20          0xF7
// #define KEY_F21          0xF8
// #define KEY_F22          0xF9
// #define KEY_F23          0xFA
// #define KEY_F24          0xFB
#define KEY_PRINT_SCREEN 0xCE
#define KEY_SCROLL_LOCK  0xCF
#define KEY_PAUSE        0xD0

// #define LED_NUMLOCK    0x01
// #define LED_CAPSLOCK   0x02
// #define LED_SCROLLLOCK 0x04
// #define LED_COMPOSE    0x08
// #define LED_KANA       0x10

DynamicJsonDocument settingsDoc(512);
String fName = "/ATOMS3U/SETTINGS.TXT";


bool waitNext = false;
long startMillisForWaitNext = 0;

int brightness = 100;


void splitString(String data, String* result) {
  int index = data.indexOf(':');
  if (index != -1) {
    result[0] = data.substring(0, index);
    result[1] = data.substring(index + 1);
  } else {
    result[0] = data;
    result[1] = "";
  }
}


void keyboardWrite(String s){
  String result[2];
  splitString(s, result);

  if(result[0] == "press"){
    String s1 = result[1];
    if(s1.length() > 0){
      if(s1 == ""){

      }else if(s1 == "KEY_LEFT_CTRL"){
        Keyboard.press(KEY_LEFT_CTRL);
      }else if(s1 == "KEY_LEFT_SHIFT"){
        Keyboard.press(KEY_LEFT_SHIFT);
      }else if(s1 == "KEY_LEFT_ALT"){
        Keyboard.press(KEY_LEFT_ALT);
      }else if(s1 == "KEY_LEFT_GUI"){
        Keyboard.press(KEY_LEFT_GUI);
      }else if(s1 == "KEY_RIGHT_CTRL"){
        Keyboard.press(KEY_RIGHT_CTRL);
      }else if(s1 == "KEY_RIGHT_SHIFT"){
        Keyboard.press(KEY_RIGHT_SHIFT);
      }else if(s1 == "KEY_RIGHT_ALT"){
        Keyboard.press(KEY_RIGHT_ALT);
      }else if(s1 == "KEY_RIGHT_GUI"){
        Keyboard.press(KEY_RIGHT_GUI);
      }else if(s1 == "KEY_UP_ARROW"){
        Keyboard.press(KEY_UP_ARROW);
      }else if(s1 == "KEY_DOWN_ARROW"){
        Keyboard.press(KEY_DOWN_ARROW);
      }else if(s1 == "KEY_LEFT_ARROW"){
        Keyboard.press(KEY_LEFT_ARROW);
      }else if(s1 == "KEY_RIGHT_ARROW"){
        Keyboard.press(KEY_RIGHT_ARROW);
      }else if(s1 == "KEY_MENU"){
        Keyboard.press(KEY_MENU);
      }else if(s1 == "KEY_SPACE"){
        Keyboard.press(KEY_SPACE);
      }else if(s1 == "KEY_BACKSPACE"){
        Keyboard.press(KEY_BACKSPACE);
      }else if(s1 == "KEY_TAB"){
        Keyboard.press(KEY_TAB);
      }else if(s1 == "KEY_RETURN"){
        Keyboard.press(KEY_RETURN);
      }else if(s1 == "KEY_ESC"){
        Keyboard.press(KEY_ESC);
      }else if(s1 == "KEY_INSERT"){
        Keyboard.press(KEY_INSERT);
      }else if(s1 == "KEY_DELETE"){
        Keyboard.press(KEY_DELETE);
      }else if(s1 == "KEY_PAGE_UP"){
        Keyboard.press(KEY_PAGE_UP);
      }else if(s1 == "KEY_PAGE_DOWN"){
        Keyboard.press(KEY_PAGE_DOWN);
      }else if(s1 == "KEY_HOME"){
        Keyboard.press(KEY_HOME);
      }else if(s1 == "KEY_END"){
        Keyboard.press(KEY_END);
      }else if(s1 == "KEY_NUM_LOCK"){
        Keyboard.press(KEY_NUM_LOCK);
      }else if(s1 == "KEY_CAPS_LOCK"){
        Keyboard.press(KEY_CAPS_LOCK);
      }else if(s1 == "KEY_F1"){
        Keyboard.press(KEY_F1);
      }else if(s1 == "KEY_F2"){
        Keyboard.press(KEY_F2);
      }else if(s1 == "KEY_F3"){
        Keyboard.press(KEY_F3);
      }else if(s1 == "KEY_F4"){
        Keyboard.press(KEY_F4);
      }else if(s1 == "KEY_F5"){
        Keyboard.press(KEY_F5);
      }else if(s1 == "KEY_F6"){
        Keyboard.press(KEY_F6);
      }else if(s1 == "KEY_F7"){
        Keyboard.press(KEY_F7);
      }else if(s1 == "KEY_F8"){
        Keyboard.press(KEY_F8);
      }else if(s1 == "KEY_F9"){
        Keyboard.press(KEY_F9);
      }else if(s1 == "KEY_F10"){
        Keyboard.press(KEY_F10);
      }else if(s1 == "KEY_F11"){
        Keyboard.press(KEY_F11);
      }else if(s1 == "KEY_F12"){
        Keyboard.press(KEY_F12);
      }else if(s1 == "KEY_F13"){
        Keyboard.press(KEY_F13);
      }else if(s1 == "KEY_F14"){
        Keyboard.press(KEY_F14);
      }else if(s1 == "KEY_F15"){
        Keyboard.press(KEY_F15);
      }else if(s1 == "KEY_F16"){
        Keyboard.press(KEY_F16);
      }else if(s1 == "KEY_F17"){
        Keyboard.press(KEY_F17);
      }else if(s1 == "KEY_F18"){
        Keyboard.press(KEY_F18);
      }else if(s1 == "KEY_F19"){
        Keyboard.press(KEY_F19);
      }else if(s1 == "KEY_F20"){
        Keyboard.press(KEY_F20);
      }else if(s1 == "KEY_F21"){
        Keyboard.press(KEY_F21);
      }else if(s1 == "KEY_F22"){
        Keyboard.press(KEY_F22);
      }else if(s1 == "KEY_F23"){
        Keyboard.press(KEY_F23);
      }else if(s1 == "KEY_F24"){
        Keyboard.press(KEY_F24);
      }else if(s1 == "KEY_PRINT_SCREEN"){
        Keyboard.press(KEY_PRINT_SCREEN);
      }else if(s1 == "KEY_SCROLL_LOCK"){
        Keyboard.press(KEY_SCROLL_LOCK);
      }else if(s1 == "KEY_PAUSE"){
        Keyboard.press(KEY_PAUSE);
      }else if(s1 == "LED_NUMLOCK"){
        Keyboard.press(LED_NUMLOCK);
      }else if(s1 == "LED_CAPSLOCK"){
        Keyboard.press(LED_CAPSLOCK);
      }else if(s1 == "LED_SCROLLLOCK"){
        Keyboard.press(LED_SCROLLLOCK);
      }else if(s1 == "LED_COMPOSE"){
        Keyboard.press(LED_COMPOSE);
      }else if(s1 == "LED_KANA"){
        Keyboard.press(LED_KANA);
      }else{
        Keyboard.press(s1[0]);
      }

    }
  }else if(result[0] == "release"){
    Keyboard.releaseAll();
  }else if(result[0] == "open"){
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('r');
    delay(100);
    Keyboard.releaseAll();
    const char* str = result[1].c_str();
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