// # This code is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino . 
#include "USB.h"

#include <M5Unified.h>
#include "led.h"
#include "Adafruit_TinyUSB.h"
#include "storage.h"
#include <ArduinoJson.h>

#include "file.h"
#include "json.h"
#include "button.h"

#ifdef EZDATA_ENABLE
#include "settings_ezdata.h"
#include "ezdata.h"
#endif

Adafruit_USBD_MSC  MSC;

void setupLog(){
#ifdef EZDATA_ENABLE
  setupEzData(ezdata_ssid, ezdata_password, ezdata_token);
#endif
}

void addLog(char * key, int i){
#ifdef EZDATA_ENABLE
    ezAddToList(key, i);
#endif
}


DynamicJsonDocument settingsDoc(512);

Adafruit_USBD_WebUSB usb_web;
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "example.tinyusb.org/webusb-serial/index.html");
void line_state_callback(bool connected)
{
  // digitalWrite(led_pin, connected);
  flickLed(2, "lime");

  if ( connected )
  {
    usb_web.println("WebUSB interface connected !!");
    usb_web.flush();
  }
}



int writeFlg = 0;
int readFlg = 0;

int contentsIndex = 3;
String getContentsStr(){
  String str = (char *) msc_disk[contentsIndex];
  return str;
}


int charas_length = 0;
int u_length = 0;

void overWriteContentsOnMemory( const char *contents){
  uint8_t * u = (uint8_t *)contents;
  int len = strlen(contents);
  for(int i = 0; i < len; i++){
    msc_disk[3][i] = u[i];
  }
}



static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){

  flickLed(2, "green");
  delay(500);
  return true;
}

int32_t mscWriteCallback (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
  uint8_t* addr = msc_disk[lba];
  memcpy(addr, buffer, bufsize);

  flickLed(2, "red");

  delay(500);
  
  writeFlg = 1;
  return bufsize;
}


int32_t mscReadCallback (uint32_t lba, void* buffer, uint32_t bufsize)
{
  uint8_t const* addr = msc_disk[lba];
  memcpy(buffer, addr, bufsize);

  flickLed(2, "blue");

  readFlg = 1;
  return bufsize;
}


void flush_cb(){
  flickLed(2, "green");
}

bool msc_ready_cb(void){
  addLog("ready", millis());
  return true;
}

String setLedStr = "";
String usbStartedStr = "";
String usbStoppedStr = "";
String usbSuspendStr = "";
String usbResumeStr = "";
String defaultEventStr = "";

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        // flickLed(2, 4);
        // liteLed("darkgreen");
        setLedStr = "orange";
        usbStartedStr = "A";
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        // flickLed(2, 5);
        // liteLed("pink");
        setLedStr = "cyan";
        usbStoppedStr = "A";
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        // liteLed("purple");
        // flickLed(2, 6);
        setLedStr = "white";
        usbSuspendStr = "A";
        break;
      case ARDUINO_USB_RESUME_EVENT:
        // flickLed(2, 7);
        // liteLed("darkcyan");
        setLedStr = "darkgreen";
        usbResumeStr = "A";
        break;
      
      default:
        defaultEventStr = "A";
        break;
    }
  }
}




void resetAndRestart(){
  removeAllFiles();
  delay(100);
  // MSC.end();
  delay(100);
  ESP.restart();
}

// bootmode
// 0: Regular Mode
// 1: Settings Mode(USB Flash)
int bootmode = 0;

// btn pressed count
// int pressedBtnCount = 0;

// bool timerIsEnabled = false;
// long startMillis = 0;
// bool checkTimerIsEnabled(int waitSeconds) {

//   long waitMillis = waitSeconds * 1000;

//   float current = millis();

//   if(!timerIsEnabled){
//     timerIsEnabled = true;
//     startMillis = current;
//   }

//   float elapsedMillis = current - startMillis;
//   float leftMillis = waitMillis - elapsedMillis;
//   float leftSeconds = leftMillis / 1000;
  

//   if(leftSeconds < 0){
//     timerIsEnabled = false;
//   }
//   return timerIsEnabled;
// }

// bool pressAndCheckBtnPressedXTimesWithinYSedonds(int x, int y){
//   pressedBtnCount = pressedBtnCount + 1;
//   bool b = false;
//   if(checkTimerIsEnabled(x)){
//     if(pressedBtnCount >= y){
//       timerIsEnabled = false;
//       pressedBtnCount = 0;
//       b = true;
//     }
//   }else{
//     pressedBtnCount = 0;
//   }
//   return b;
// }


// regular mode settings >>

//// initial settings
char * initialContents = R"({"settings_mode": "storage", "color": "red"})";
String settings_mode = "storage";
int requiresResetInSettingsMode = 0;

//// regular code in setup
void setupInRegularMode(){
  USB.begin();
}
//// write regular code in loop
void loopInRegularMode(){
  if (M5.BtnA.wasPressed()) {
    DynamicJsonDocument doc = getJsonDocumentFromFile(fileName);
    if(doc.containsKey("color")){
      auto color1 = doc["color"].as<const char*>();
    }

    if(settingsDoc.containsKey("color")){
      String color_1 = settingsDoc["color"].as<String>();
      offLed();
      delay(10);
      liteLed(color_1);
    }else{
      flickLed(2, "white");
    }
  }
}

String receivedString = "";
String setMode = "";// "", "key", "value"
String setKey = "";
String setValue = "";
void echo_all(uint8_t buf[], uint32_t count)
{
  flickLed(2, "yellow");

  if (usb_web.connected()){
    receivedString += String((char*)buf);
    int entered = 0;
    usb_web.write(buf, count);
    for(uint32_t i=0; i<count; i++){
      if ( buf[i] == '\r' || buf[i] == '\n'){
        usb_web.flush();
        entered = 1;
      }
    }
    if(entered == 1){
      if(setMode == "key"){
        receivedString.remove(receivedString.length() - 1);
        setKey = receivedString;
        setMode = "value";
        usb_web.println("set mode please type value");
      }else if(setMode == "value"){
        receivedString.remove(receivedString.length() - 1);
        setValue = receivedString;
        
        settingsDoc = setKeyValueToJson(settingsDoc, setKey, setValue);
        
        setMode = "";
        setKey = "";
        setValue = "";

        String settings_str = getJsonString(settingsDoc);

        writeToFile(settings_str);
        
        usb_web.println(settings_str);
      }

      if(receivedString == "get mode\r" || receivedString == "get mode\n"){
        usb_web.println(settings_mode);
      }else if(receivedString == "get initial\r" || receivedString == "get initial\n"){
        usb_web.println(initialContents);
      }else if(receivedString == "get settings\r" || receivedString == "get settings\n"){
        String settings_str = getJsonString(settingsDoc);
        usb_web.println(settings_str);
      }else if(receivedString == "set\r" || receivedString == "set\n"){
        setMode = "key";
        usb_web.println("set mode please type key");
      }else{
        usb_web.println(receivedString);
      }
      usb_web.flush();

      if(receivedString == "reset\r" || receivedString == "reset\n"){
        requiresResetInSettingsMode = 1;
      }

      receivedString = "";
    }
  }

  // if ( Serial )
  // {
  //   int entered = 0;
  //   for(uint32_t i=0; i<count; i++){
  //     // Serial.write(buf[i]);
  //     if ( buf[i] == '\r' ){
  //       entered = 1;
  //     }
  //   }
    
  //   if(entered == 1){
  //     if(receivedString == "get"){
  //       Serial.write("AAAA");
  //     }else{
  //       Serial.write(initialContents);
  //     }

  //     Serial.write('\n');
  //     receivedString = "";
  //     Serial.flush();
  //     entered = 0;
  //   }
  // }


}

// regular mode settings <<

void setupInSettingsMode(){
    if(settingsDoc.containsKey("settings_mode")){
      settings_mode = settingsDoc["settings_mode"].as<String>();
    }

    if(settings_mode == "storage"){
      // storage
      USB.onEvent(usbEventCallback);

      MSC.setID("M5AtomS3U", "USBD_MSC", "1.0");
      MSC.setCapacity(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
      // MSC.setReadyCallback(msc_ready_cb);
      MSC.setStartStopCallback(onStartStop);
      MSC.setReadWriteCallback(mscReadCallback, mscWriteCallback, flush_cb);
      MSC.setUnitReady(true);
      MSC.begin();

      USB.begin();
    }else if(settings_mode == "web"){
      // webserial
      USB.begin();
      usb_web.setLandingPage(&landingPage);
      usb_web.setLineStateCallback(line_state_callback);
      while(!usb_web.begin()){
        flickLed(2, "magenta");
        delay(1);
      }

      // wait until device mounted
      while( !TinyUSBDevice.mounted() ){
        flickLed(2, "red");
        delay(1);
      }

      if( TinyUSBDevice.mounted() ){
        flickLed(2, "cyan");
      }
    }
}

void loopInSettingsMode(){
  if(settings_mode == "storage"){
    // addLog("loopInSettingsModeStorage", millis());
    if(writeFlg == 1){
      int targetSize = sizeof(msc_disk[3]);
      String str = (char *) msc_disk[3];
      int strsize = str.length();
      delay(100);

      writeToFile(str);
      delay(100);

      writeFlg = 0;
    }
    if(readFlg == 1){
      readFlg = 0;
    }

    if(setLedStr != ""){
      char result[10];
      setLedStr.toCharArray(result, 10);
      addLog(result, millis());

      liteLed(setLedStr);
      setLedStr = "";
    }


    if(readFlg != 1 && writeFlg != 1 && M5.BtnA.wasPressed()){
      // If you press the button five times within 5 seconds, it will delete the files in the ROM area and restart. The configuration JSON will be reset to its initial state.

      if(pressAndCheckBtnPressedXTimesWithinYSedonds(5, 5)){
        requiresResetInSettingsMode = 1;
      }
    }
  }else if(settings_mode == "web"){
    // addLog("loopInSettingsModeWeb", millis());

    uint8_t buf[64];
    uint32_t count;
    if (Serial.available())
    {
      count = Serial.read(buf, 64);
      flickLed(2, "red");
      echo_all(buf, count);
    }

    // from WebUSB to both Serial & webUSB
    if (usb_web.available())
    {
      flickLed(2, "green");
      count = usb_web.read(buf, 64);
      echo_all(buf, count);
    }
  }

  if(requiresResetInSettingsMode == 1){
    resetAndRestart();
  }
  

  if(usbStartedStr != ""){
    // addLog("usbStartedStr", millis());
    usbStartedStr = "";
  }
  if(usbStoppedStr != ""){
    // addLog("usbStoppedStr", millis());
    usbStoppedStr = "";
  }
  if(usbSuspendStr != ""){
    // addLog("usbSuspendStr", millis());
    usbSuspendStr = "";
  }
  if(usbResumeStr != ""){
    // addLog("usbResumeStr", millis());
    usbResumeStr = "";
  }
  
  if(defaultEventStr != ""){
    // addLog("defaultEventStr", millis());
    defaultEventStr = "";
  }


}

void initRomArea(char * initialContents){

  File dataFile = initAndGetDataFile(initialContents, fileName);
  String readStr = dataFile.readString();
  char Buf[DISK_SECTOR_SIZE];
  readStr.toCharArray(Buf, DISK_SECTOR_SIZE);
  initialContents = Buf;
  dataFile.close();
  settingsDoc = getJsonDocumentFromFile(fileName);

  // delay(100);
  // addLog("initRomArea", millis());

  overWriteContentsOnMemory(initialContents);

}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);


  auto cfg = M5.config();
  M5.begin(cfg);
  initLed();
  M5.Power.setLed(0);

  int beforeSetupWiFiMilli = millis();
#ifdef EZDATA_ENABLE
  setupLog();
#endif
  // addLog("mscram_start0", beforeSetupWiFiMilli);
  // addLog("mscram_start1", millis());



  if (M5.BtnA.wasPressed()) {
    bootmode = 1;
  }

  initRomArea(initialContents);

  // addLog("setup0bootmode", bootmode);

  if(bootmode == 0){// 1. Regular Mode
    setupInRegularMode();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    setupInSettingsMode();
  }


  // addLog("setup1bootmode", bootmode);


}

void loop() {

  M5.update();

  if(bootmode == 0){// 1. Regular Mode
    loopInRegularMode();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    loopInSettingsMode();
  }


}

