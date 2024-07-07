#include "USB.h"
#include "Adafruit_TinyUSB.h"
#include "led.h"


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