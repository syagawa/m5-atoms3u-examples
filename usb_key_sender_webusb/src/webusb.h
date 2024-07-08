#ifndef WEBUSB_H
#define WEBUSB_H

#include "USB.h"
#include "Adafruit_TinyUSB.h"
#include "led.h"
#include "json.h"


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

DynamicJsonDocument settingsDoc(512);


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



}



void setupInSettingsMode(){

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


void loopInSettingsMode(){

  
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



#endif //WEBUSB_H