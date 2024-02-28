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