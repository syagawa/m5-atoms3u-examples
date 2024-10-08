// # This code is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino . 
#include "USB.h"

#include <M5Unified.h>
#include "led.h"
#include "Adafruit_TinyUSB.h"
#include "ArduinoJson.h"

//// initial settings
DynamicJsonDocument settingsDocInMain(512);
char * initialContents = R"({"color": "red", "colors": ["blue", "yellow"]})";


#include "json.h"


#include "file.h"
#include "button.h"


#include "app.h"
#include "webusb.h"


int writeFlg = 0;
int readFlg = 0;

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  flickLed(2, "green");
  delay(500);
  return true;
}


String setLedStr = "";


void resetAndRestart(){
  flickLed(3, "red");
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

//// regular code in setup
void setupInRegularMode(){
  settingsApp();
  // Keyboard.begin();
  // USB.begin();
}


//// write regular code in loop
void loopInRegularMode(){
  loopApp(M5.BtnA.wasPressed(), M5.BtnA.pressedFor(1000));
}


// regular mode settings <<


void loopInSettingsMode(){

  uint8_t buf[64];
  uint32_t count;
  if (Serial.available()){
    count = Serial.read(buf, 64);
    flickLed(2, "red");
    echo_all(buf, count);
  }

  // from WebUSB to both Serial & webUSB
  if (usb_web.available()){
    flickLed(2, "green");
    count = usb_web.read(buf, 64);
    echo_all(buf, count);
  }
  

}


void setup() {
  // Serial.begin(115200);
  // Serial.setDebugOutput(true);

  auto cfg = M5.config();
  M5.begin(cfg);
  initLed();
  M5.Power.setLed(0);

  int beforeSetupWiFiMilli = millis();

  if (M5.BtnA.wasPressed()) {
    bootmode = 1;
  }

  initRomArea(initialContents, false);

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

