// # This code is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino . 
#include "USB.h"
#include "USBMSC.h"

#include <M5Unified.h>
#include "led.h"

#include "json.h"

#include "storage.h"

#include "file.h"
#include "button.h"

USBMSC MSC;

#include "app.h"


int writeFlg = 0;
int readFlg = 0;

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  flickLed(2, "green");
  delay(500);
  return true;
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
  memcpy(msc_disk[lba] + offset, buffer, bufsize);
  flickLed(2, "red");
  writeFlg = 1;
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  memcpy(buffer, msc_disk[lba] + offset, bufsize);
  flickLed(2, "yellow");
  readFlg = 1;
  return bufsize;
}

String setLedStr = "";

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        setLedStr = "orange";
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        setLedStr = "cyan";
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        setLedStr = "white";
        break;
      case ARDUINO_USB_RESUME_EVENT:
        setLedStr = "darkgreen";
        break;
      default:
        break;
    }
  }
}

void resetAndRestart(){
  removeAllFiles();
  delay(100);
  delay(100);
  ESP.restart();
}

// bootmode
// 0: Regular Mode
// 1: Settings Mode(USB Flash)
int bootmode = 0;

// regular mode settings >>

//// initial settings
char * initialContents = R"({"color": "red", "keys": ["abc"], "waitSeconds": 3})";
int requiresResetInSettingsMode = 0;

//// regular code in setup
void setupInRegularMode(){
  settingsApp();
  Keyboard.begin();
  USB.begin();
}


//// write regular code in loop
void loopInRegularMode(){
  loopApp();
}


// regular mode settings <<

void setupInSettingsMode(){
    // storage
    USB.onEvent(usbEventCallback);
    MSC.vendorID("M5AtomS3U");//max 8 chars
    MSC.productID("USB_MSC");//max 16 chars
    MSC.productRevision("1.0");//max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);
    MSC.mediaPresent(true);
    MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);

    USB.begin();
}

void loopInSettingsMode(){
  
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

    liteLed(setLedStr);
    setLedStr = "";
  }

  if(readFlg != 1 && writeFlg != 1 && M5.BtnA.wasPressed()){
    // If you press the button five times within 5 seconds, it will delete the files in the ROM area and restart. The configuration JSON will be reset to its initial state.
    if(pressAndCheckBtnPressedXTimesWithinYSedonds(5, 5)){
      requiresResetInSettingsMode = 1;
    }
  }
  

  if(requiresResetInSettingsMode == 1){
    resetAndRestart();
  }

}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  auto cfg = M5.config();
  M5.begin(cfg);
  initLed();
  M5.Power.setLed(0);

  int beforeSetupWiFiMilli = millis();

  if (M5.BtnA.wasPressed()) {
    bootmode = 1;
  }

  initRomArea(initialContents);

  if(bootmode == 0){// 1. Regular Mode
    setupInRegularMode();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    setupInSettingsMode();
  }
}

void loop() {
  M5.update();
  if(bootmode == 0){// 1. Regular Mode
    loopInRegularMode();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    loopInSettingsMode();
  }
}

