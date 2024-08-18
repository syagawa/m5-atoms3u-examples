#ifndef WEBUSB_H
#define WEBUSB_H

#include "Adafruit_TinyUSB.h"
#include "led.h"
#include "json.h"
#include "file.h"


Adafruit_USBD_WebUSB usb_web;
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "example.tinyusb.org/webusb-serial/index.html");

Adafruit_USBD_CDC USBSerial;

void line_state_callback(bool connected)
{
  flickLed(2, "lime");

  if ( connected )
  {
    usb_web.println("WebUSB interface connected !!");
    usb_web.flush();
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
      }else if(setMode == "push"){
        receivedString.remove(receivedString.length() - 1);
        setKey = receivedString;
        setMode = "pushvalue";
        usb_web.println("push mode please type value");
      }else if(setMode == "value"){
        receivedString.remove(receivedString.length() - 1);
        setValue = receivedString;
        
        settingsDocInMain = setKeyValueToJson(settingsDocInMain, setKey, setValue);
        
        setMode.clear();
        setKey.clear();
        setValue.clear();

        String settings_str = getJsonString(settingsDocInMain);
        writeToFile(settings_str);
        usb_web.println(settings_str);
      }else if(setMode == "pushvalue"){
        receivedString.remove(receivedString.length() - 1);
        setValue = receivedString;
        
        settingsDocInMain = pushValueByKeyToJson(settingsDocInMain, setKey, setValue);
        
        setMode.clear();
        setKey.clear();
        setValue.clear();

        String settings_str = getJsonString(settingsDocInMain);
        writeToFile(settings_str);
        usb_web.println(settings_str);
      }else if(setMode == "pop"){
        receivedString.remove(receivedString.length() - 1);
        setKey = receivedString;
        
        settingsDocInMain = popValueByKeyToJson(settingsDocInMain, setKey);
        
        setMode.clear();
        setKey.clear();
        setValue.clear();

        String settings_str = getJsonString(settingsDocInMain);
        writeToFile(settings_str);
        usb_web.println(settings_str);
      }else if(setMode == "remove"){
        receivedString.remove(receivedString.length() - 1);
        setKey = receivedString;
        settingsDocInMain = removeKeyValueInJson(settingsDocInMain, setKey);

        setMode.clear();
        setKey.clear();
        setValue.clear();

        String settings_str = getJsonString(settingsDocInMain);
        writeToFile(settings_str);
        usb_web.println(settings_str);
      }

      if(receivedString == "get initial\r" || receivedString == "get initial\n"){
        usb_web.println(initialContents);
      }else if(receivedString == "get settings\r" || receivedString == "get settings\n"){
        settingsDocInMain = getJsonDocumentFromFile(fileName);
        String settings_str = getJsonString(settingsDocInMain);
        usb_web.println(settings_str);
      }else if(receivedString == "set\r" || receivedString == "set\n"){
        setMode = "key";
        usb_web.println("set mode please type key");
      }else if(receivedString == "push\r" || receivedString == "push\n"){
        setMode = "push";
        usb_web.println("push mode please type key");
      }else if(receivedString == "pop\r" || receivedString == "pop\n"){
        setMode = "pop";
        usb_web.println("pop mode please type key");
      }else if(receivedString == "remove\r" || receivedString == "remove\n"){
        setMode = "remove";
        usb_web.println("remove mode please type remove key");
      }else if(receivedString == "reset\r" || receivedString == "reset\n"){
        initRomArea(initialContents, true);
        String settings_str = getJsonString(settingsDocInMain);
        usb_web.println(settings_str);
      }else{
        usb_web.println(receivedString);
      }
      usb_web.flush();


      receivedString.clear();
    }
  }

}



void setupInSettingsMode(){

  USB.productName("M5AtomS3U storage webusb");
  USB.manufacturerName("M5Stack");
  USB.begin();

  usb_web.setLandingPage(&landingPage);
  usb_web.setLineStateCallback(line_state_callback);

  while(!usb_web.begin()){

    flickLed(2, "red");
    delay(1);
  }
  while( !TinyUSBDevice.mounted() ){
    flickLed(2, "green");
    delay(1);
  }

  if( TinyUSBDevice.mounted() ){
    flickLed(2, "cyan");
  }

}


#endif //WEBUSB_H