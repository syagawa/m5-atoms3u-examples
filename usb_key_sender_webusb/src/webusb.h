#ifndef WEBUSB_H
#define WEBUSB_H

// #include "USB.h"
#include "Adafruit_TinyUSB.h"
#include "led.h"
#include "json.h"
#include "file.h"


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

// char * initialContents = R"({"color": "red"})";


String receivedString = "";
String setMode = "";// "", "key", "value"
String setKey = "";
String setValue = "";
void echo_all(uint8_t buf[], uint32_t count, DynamicJsonDocument settingsDoc, int requiresResetInSettingsMode, char* initialContents)
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

      if(receivedString == "get initial\r" || receivedString == "get initial\n"){
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
  // usb_web.begin();
  // if (!TinyUSBDevice.isInitialized()) {
  //   TinyUSBDevice.begin(0);
  // }
  
  // if (!TinyUSBDevice.ready()) {
  //   TinyUSBDevice.begin(0);
  // }

  // if (!TinyUSBDevice.mounted()) {
  //   TinyUSBDevice.begin(0);
  // }


  // USB.begin();
  // delay(100);

  usb_web.setLandingPage(&landingPage);
  usb_web.setLineStateCallback(line_state_callback);

  // TinyUSBDevice.begin(0);
  usb_web.begin();
  // while(!usb_web.begin()){
  //   flickLed(2, "magenta");
  //   delay(1);
  // }
  // usb_web.begin();
  // Serial.begin(115200);

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ){
    flickLed(2, "green");
    // TinyUSBDevice.begin(0);
    // usb_web.begin();
    delay(1);
  }

  if( TinyUSBDevice.mounted() ){
    flickLed(2, "cyan");
  }
    
}






#endif //WEBUSB_H