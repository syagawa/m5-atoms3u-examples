// # This code is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino . 
#include "USB.h"
#include "USBMSC.h"

#include <M5Unified.h>

#include "led.h"

#include <EEPROM.h>

// #define HWSerial Serial
#define HWSerial Serial(2)
USBCDC USBSerial;


USBMSC MSC;

#include "storage.h"

#include <SPIFFS.h>
#include <ArduinoJson.h>

class Exception
{
  private:
  const char* _msg;
  public:
  Exception(const char* msg)
  {
    _msg = msg;
  }

  const char* Message() const
  {
    return _msg;
  }
};


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



static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize){
  USBSerial.printf("1MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  USBSerial.printf("2MSC WRITE buffer: %u\n", buffer);

  // memcpy(buf1, buf2, n)
  // void *buf1: Copy Destination Memory Block
  // const void *buf2: Copy Source Memory Block
  // size_t n: Number of Bytes to Copy
  // Is this memcpy not working? Even without this line, SETTINGS.TXT is displayed and data can be updated as long as the power is on.
  memcpy(msc_disk[lba] + offset, buffer, bufsize);

  flickLed(2, "red");

  delay(500);

  writeFlg = 1;
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  USBSerial.printf("1MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  USBSerial.printf("2MSC READ buffer: %u\n", buffer);

  // memcpy(buf1, buf2, n)
  // void *buf1: Copy Destination Memory Block
  // const void *buf2: Copy Source Memory Block
  // size_t n: Number of Bytes to Copy
  // This memcpy is working. Without this line, SETTINGS.TXT file and storage won't be displayed
  memcpy(buffer, msc_disk[lba] + offset, bufsize);

  flickLed(2, "yellow");

  readFlg = 1;

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  USBSerial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

  delay(500);
  flickLed(2, "magenta");
  return true;
}

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
  if(event_base == ARDUINO_USB_EVENTS){
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id){
      case ARDUINO_USB_STARTED_EVENT:
        USBSerial.println("USB PLUGGED");
        // flickLed(2, 4);
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        USBSerial.println("USB UNPLUGGED");
        // flickLed(2, 5);
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        USBSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        // flickLed(2, 6);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        USBSerial.println("USB RESUMED");
        // flickLed(2, 7);
        break;
      
      default:
        break;
    }
  }
}


String fileName = "/ATOMS3U/SETTINGS.TXT";
bool SPIFFS_FORMAT = true;

void writeToFile(String str){
  File file = SPIFFS.open(fileName, "w");
  file.print(str);
  file.close();
}

DynamicJsonDocument getJsonDocumentFromFile(String fileName){
  DynamicJsonDocument doc(512);
  try{
    File file = SPIFFS.open(fileName, "r");
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if(error){
      USBSerial.printf("doc file error %s \n", error.c_str());
    }
  }catch(Exception& ex){
    USBSerial.printf("json error %s \n", ex.Message());
  }
  return doc;

}

void listAllFiles(){
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
 
      USBSerial.print("FILE: ");
      USBSerial.println(file.path());

      file = root.openNextFile();
  }
}

void removeAllFiles(){
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
      SPIFFS.remove(file.path());
      file = root.openNextFile();
  }
}

void resetAndRestart(){
  removeAllFiles();
  delay(100);
  MSC.end();
  delay(100);
  ESP.restart();
}

// bootmode
// 0: Regular Mode
// 1: Settings Mode(USB Flash)
int bootmode = 0;
DynamicJsonDocument settingsDoc(512);

// btn pressed count
int pressedBtnCount = 0;

bool timerIsEnabled = false;
long startMillis = 0;
bool checkTimerIsEnabled(int waitSeconds) {

  long waitMillis = waitSeconds * 1000;

  float current = millis();

  if(!timerIsEnabled){
    timerIsEnabled = true;
    startMillis = current;
  }

  float elapsedMillis = current - startMillis;
  float leftMillis = waitMillis - elapsedMillis;
  float leftSeconds = leftMillis / 1000;
  
  USBSerial.printf("leftSeconds: %g \n", leftSeconds);

  if(leftSeconds < 0){
    timerIsEnabled = false;
  }
  return timerIsEnabled;
}

bool pressAndCheckBtnPressedXTimesWithinYSedonds(int x, int y){
  pressedBtnCount = pressedBtnCount + 1;
  USBSerial.printf("pressedBtnCount: %u \n", pressedBtnCount);
  bool b = false;
  if(checkTimerIsEnabled(x)){
    if(pressedBtnCount >= y){
      timerIsEnabled = false;
      pressedBtnCount = 0;
      b = true;
    }
  }else{
    pressedBtnCount = 0;
  }
  return b;
}


// regular mode settings >>

//// initial settings
char * initialContents = R"({"color": "red"})";

//// write regular code in setup
void regularInSetup(){
  USBSerial.begin();
  USB.begin();
}

//// write regular code in loop
void regularInLoop(){
  if (M5.BtnA.wasPressed()) {
    USBSerial.println("pressed!!");
    DynamicJsonDocument doc = getJsonDocumentFromFile(fileName);
    if(doc.containsKey("color")){
      auto color1 = doc["color"].as<const char*>();
      USBSerial.printf("doc color1: %s \n", color1);
    }

    if(settingsDoc.containsKey("color")){
      String color_1 = settingsDoc["color"].as<String>();
      USBSerial.printf("color_1: %s %c \n", color_1, color_1);
      flickLed(2, color_1);
    
    }else{
      flickLed(2, "black");
    }
  }
}

// regular mode settings <<

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  auto cfg = M5.config();
  M5.begin(cfg);
  initLed();
  M5.Power.setLed(0);

  if (M5.BtnA.wasPressed()) {
    bootmode = 1;
  }

  // 0. initialize Rom area 
  if(SPIFFS.begin()){
    File dataFile;
    if(!SPIFFS.exists(fileName)){
      dataFile = SPIFFS.open(fileName, "w");
      dataFile.print(initialContents);
      dataFile.close();
    }

    dataFile = SPIFFS.open(fileName, "r");
    String readStr = dataFile.readString();
    char Buf[DISK_SECTOR_SIZE];
    readStr.toCharArray(Buf, DISK_SECTOR_SIZE);
    initialContents = Buf;
    dataFile.close();
    settingsDoc = getJsonDocumentFromFile(fileName);
  }

  overWriteContentsOnMemory(initialContents);

  if(bootmode == 0){// 1. Regular Mode
    regularInSetup();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    USB.onEvent(usbEventCallback);
    MSC.vendorID("ESP32");//max 8 chars
    MSC.productID("USB_MSC");//max 16 chars
    MSC.productRevision("1.0");//max 4 chars
    MSC.onStartStop(onStartStop);
    MSC.onRead(onRead);
    MSC.onWrite(onWrite);
    MSC.mediaPresent(true);
    MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
    USBSerial.begin();
    USB.begin();
  }
}

void loop() {

  M5.update();


  if(bootmode == 0){// 1. Regular Mode
    regularInLoop();
  }else if(bootmode == 1){// 2. Settings Mode(USB Flash)
    if(writeFlg == 1){

      int targetSize = sizeof(msc_disk[3]);
      String str = (char *) msc_disk[3];
      int strsize = str.length();
      USBSerial.printf("7loop WRITE: msc_disk targetSize: %u, contents strsize: %u, str: %s \n", targetSize, strsize, str);
      delay(100);

      writeToFile(str);
      delay(100);

      writeFlg = 0;
    }
    if(readFlg == 1){
      readFlg = 0;
    }

    if(readFlg != 1 && writeFlg != 1 && M5.BtnA.wasPressed()){
      // If you press the button five times within 5 seconds, it will delete the files in the ROM area and restart. The configuration JSON will be reset to its initial state.
      if(pressAndCheckBtnPressedXTimesWithinYSedonds(5, 5)){
        resetAndRestart();
      }
    }

  }


}

