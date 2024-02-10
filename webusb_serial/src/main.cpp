/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/* This sketch demonstrates WebUSB as web serial with browser with WebUSB support (e.g Chrome).
 * After enumerated successfully, Browser will pop-up notification
 * with URL to landing page, click on it to test
 *  - Click "Connect" and select device, When connected the on-board LED will litted up.
 *  - Any charters received from either webusb/Serial will be echo back to webusb and Serial
 *  
 * Note: 
 * - The WebUSB landing page notification is currently disabled in Chrome 
 * on Windows due to Chromium issue 656702 (https://crbug.com/656702). You have to 
 * go to landing page (below) to test
 * 
 * - On Windows 7 and prior: You need to use Zadig tool to manually bind the 
 * WebUSB interface with the WinUSB driver for Chrome to access. From windows 8 and 10, this
 * is done automatically by firmware.
 */
#include <M5Unified.h>
#include "USB.h"
#include "Adafruit_TinyUSB.h"
#include <EspEasyLED.h>

EspEasyLED *rgbled;

USBCDC USBSerial;

Adafruit_USBD_WebUSB usb_web;

// Landing Page: scheme (0: http, 1: https), url
// Page source can be found at https://github.com/hathach/tinyusb-webusb-page/tree/main/webusb-serial
WEBUSB_URL_DEF(landingPage, 1 /*https*/, "example.tinyusb.org/webusb-serial/index.html");


int led_pin = LED_BUILTIN;

void offLed(){
  rgbled->setBrightness(0);
  rgbled->show();
}

void onLed(){
  rgbled->setBrightness(100);
  rgbled->show();
}


void flickLed(int num, String c){

  onLed();
  for(int i = 0; i < num; i++){
    if(c == "red"){
      rgbled->showColor(255, 0, 0);//red
    }else if(c == "yellow"){
      rgbled->showColor(255, 255, 0);//yellow
    }else if(c == "magenta"){
      rgbled->showColor(255, 0, 225);//magenta
    }else if(c == "lime"){
      rgbled->showColor(0, 255, 0);//lime
    }else if(c == "blue"){
      rgbled->showColor(0, 0, 225);//blue
    }else if(c == "cyan"){
      rgbled->showColor(0, 255, 225);//cyan
    }else{
      rgbled->showColor(0, 0, 0);//black
    }
    delay(100);
    rgbled->showColor(255, 255, 255);//white
    delay(100);
  }
  offLed();

}

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


// the setup function runs once when you press reset or power the board
void setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  auto cfg = M5.config();
  M5.begin(cfg);
  rgbled = new EspEasyLED(GPIO_NUM_35, 1, 20);
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
    Serial.println("not mounted 2");
    delay(1);
  }

  if( TinyUSBDevice.mounted() ){
    flickLed(2, "cyan");
    Serial.println("mounted 2");
  }

}

// function to echo to both Serial and WebUSB
void echo_all(uint8_t buf[], uint32_t count)
{
  flickLed(2, "yellow");

  if (usb_web.connected())
  {
    flickLed(2, "lime");
    usb_web.write(buf, count);
    usb_web.flush();
  }

  if ( Serial )
  {
    flickLed(2, "cyan");
    for(uint32_t i=0; i<count; i++)
    {
      Serial.write(buf[i]);
      if ( buf[i] == '\r' ) Serial.write('\n');
    }
    Serial.flush();
  }
}

void loop(){
  M5.update();
  uint8_t buf[64];
  uint32_t count;


  if(M5.BtnA.wasPressed()){
    Serial.println("pressed 2");

    if( TinyUSBDevice.mounted() ){
      flickLed(2, "lime");
    }else{
      flickLed(2, "magenta");
    }

    if(usb_web.connected()){
      flickLed(2, "cyan");
    }else{
      flickLed(2, "magenta");
    }
    
    if(usb_web.available()){
      flickLed(2, "blue");
    }else{
      flickLed(2, "magenta");
    }


    usb_web.setLandingPage(&landingPage);
    usb_web.setLineStateCallback(line_state_callback);
    usb_web.begin();

  }

  Serial.println("in loop 2");


  // From Serial to both Serial & webUSB
  if (Serial.available())
  {
    count = Serial.read(buf, 64);
    echo_all(buf, count);
  }

  // from WebUSB to both Serial & webUSB
  if (usb_web.available())
  {
    count = usb_web.read(buf, 64);
    echo_all(buf, count);
  }
}
