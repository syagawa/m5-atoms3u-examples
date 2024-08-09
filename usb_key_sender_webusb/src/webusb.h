#ifndef WEBUSB_H
#define WEBUSB_H

// #include "USB.h"
#include "Adafruit_TinyUSB.h"
#include "led.h"
#include "json.h"
#include "file.h"


Adafruit_USBD_WebUSB usb_web;
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "example.tinyusb.org/webusb-serial/index.html");

// USBCDC USBSerial;

Adafruit_USBD_CDC USBSerial;

// Adafruit_USBD_MSC MSC;

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
      }else{
        usb_web.println(receivedString);
      }
      usb_web.flush();

      if(receivedString == "reset\r" || receivedString == "reset\n"){
        requiresResetInSettingsMode = 1;
      }

      receivedString.clear();
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

  // MSC.setID("M5AtomS3U", "USBD_MSC", "1.0");
  // MSC.begin();


  // USBSerial.begin();
  USB.productName("M5AtomS3U USB Key Sender");
  USB.manufacturerName("M5Stack");
  USB.begin();
  // delay(100);


  usb_web.setLandingPage(&landingPage);
  usb_web.setLineStateCallback(line_state_callback);

  // TinyUSBDevice.begin(0);
  // usb_web.begin();
  // USBSerial.println("AAA0");

  while(!usb_web.begin()){
    // USBSerial.println("ccc0");

    flickLed(2, "red");
    delay(1);
  }
  // usb_web.begin();
  // Serial.begin(115200);

  // wait until device mounted
  // USBSerial.println("AAA1");
  while( !TinyUSBDevice.mounted() ){
    // USBSerial.println("ccc1");
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