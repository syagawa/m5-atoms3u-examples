// # This code is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino . 
#include "USB.h"

#include <M5Unified.h>
#include "led.h"
#include "Adafruit_TinyUSB.h"

#include "json.h"

DynamicJsonDocument settingsDoc(512);

#include "storage.h"
#include <ArduinoJson.h>

#include "file.h"
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


int writeFlg = 0;
int readFlg = 0;

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
        setLedStr = "orange";
        usbStartedStr = "A";
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        setLedStr = "cyan";
        usbStoppedStr = "A";
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        setLedStr = "white";
        usbSuspendStr = "A";
        break;
      case ARDUINO_USB_RESUME_EVENT:
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


// regular mode settings >>

//// initial settings
char * initialContents = R"({"settings_mode": "storage", "color": "red"})";
String settings_mode = "storage";
int requiresResetInSettingsMode = 0;

//// regular code in setup
void setupInRegularMode(){
  USB.begin();
}

int brightness = 100;
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
      liteLed(color_1, brightness);
      brightness = brightness - 10;
      if(brightness < 0){
        brightness = 100;
      }
    }else{
      flickLed(2, "white");
    }
  }
}

String receivedString = "";
String setMode = "";// "", "key", "value"
String setKey = "";
String setValue = "";
void echo_all(uint8_t buf[], uint32_t count){
  flickLed(2, "yellow");
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

// void initRomArea(char * initialContents){

//   File dataFile = initAndGetDataFile(initialContents, fileName);
//   String readStr = dataFile.readString();
//   char Buf[DISK_SECTOR_SIZE];
//   readStr.toCharArray(Buf, DISK_SECTOR_SIZE);
//   initialContents = Buf;
//   dataFile.close();
//   settingsDoc = getJsonDocumentFromFile(fileName);

//   // delay(100);
//   // addLog("initRomArea", millis());

//   overWriteContentsOnMemory(initialContents);

// }

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

