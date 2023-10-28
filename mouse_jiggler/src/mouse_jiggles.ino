// https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino
#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup(){}
void loop(){}
#else
#include "USB.h"
#include "USBMSC.h"
#include "USBHIDMouse.h"

#include <M5Unified.h>
#include <EspEasyLED.h>
EspEasyLED *rgbled;


#include <EEPROM.h>

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
// #define HWSerial Serial
#define HWSerial Serial(2)
USBCDC USBSerial;
#endif

USBMSC MSC;

#define FAT_U8(v) ((v) & 0xFF)
#define FAT_U16(v) FAT_U8(v), FAT_U8((v) >> 8)
#define FAT_U32(v) FAT_U8(v), FAT_U8((v) >> 8), FAT_U8((v) >> 16), FAT_U8((v) >> 24)
#define FAT_MS2B(s,ms)    FAT_U8(((((s) & 0x1) * 1000) + (ms)) / 10)
#define FAT_HMS2B(h,m,s)  FAT_U8(((s) >> 1)|(((m) & 0x7) << 5)),      FAT_U8((((m) >> 3) & 0x7)|((h) << 3))
#define FAT_YMD2B(y,m,d)  FAT_U8(((d) & 0x1F)|(((m) & 0x7) << 5)),    FAT_U8((((m) >> 3) & 0x1)|((((y) - 1980) & 0x7F) << 1))
#define FAT_TBL2B(l,h)    FAT_U8(l), FAT_U8(((l >> 8) & 0xF) | ((h << 4) & 0xF0)), FAT_U8(h >> 4)


// space * 511
#define DEFAULT_CONTENTS "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               "

static const uint32_t DISK_SECTOR_COUNT = 2 * 8; // 8KB is the smallest size that windows allow to mount
static const uint16_t DISK_SECTOR_SIZE = 512;    // Should be 512
static const uint16_t DISC_SECTORS_PER_TABLE = 1; //each table sector can fit 170KB (340 sectors)

static uint8_t msc_disk[DISK_SECTOR_COUNT][DISK_SECTOR_SIZE] =
{
  //------------- Block0: Boot Sector -------------//
  {
    // Header (62 bytes)
    0xEB, 0x3C, 0x90, //jump_instruction
    'M' , 'S' , 'D' , 'O' , 'S' , '5' , '.' , '0' , //oem_name
    FAT_U16(DISK_SECTOR_SIZE), //bytes_per_sector
    FAT_U8(1),    //sectors_per_cluster
    FAT_U16(1),   //reserved_sectors_count
    FAT_U8(1),    //file_alloc_tables_num
    FAT_U16(16),  //max_root_dir_entries
    FAT_U16(DISK_SECTOR_COUNT), //fat12_sector_num
    0xF8,         //media_descriptor
    FAT_U16(DISC_SECTORS_PER_TABLE),   //sectors_per_alloc_table;//FAT12 and FAT16
    FAT_U16(1),   //sectors_per_track;//A value of 0 may indicate LBA-only access
    FAT_U16(1),   //num_heads
    FAT_U32(0),   //hidden_sectors_count
    FAT_U32(0),   //total_sectors_32
    0x00,         //physical_drive_number;0x00 for (first) removable media, 0x80 for (first) fixed disk
    0x00,         //reserved
    0x29,         //extended_boot_signature;//should be 0x29
    FAT_U32(0x1234), //serial_number: 0x1234 => 1234
    'T' , 'i' , 'n' , 'y' , 'U' , 'S' , 'B' , ' ' , 'M' , 'S' , 'C' , //volume_label padded with spaces (0x20)
    'F' , 'A' , 'T' , '1' , '2' , ' ' , ' ' , ' ' ,  //file_system_type padded with spaces (0x20)

    // Zero up to 2 last bytes of FAT magic code (448 bytes)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     //boot signature (2 bytes)
    0x55, 0xAA
  },

  //------------- Block1: FAT12 Table -------------//
  {
    FAT_TBL2B(0xFF8, 0xFFF), FAT_TBL2B(0xFFF, 0x000) // first 2 entries must be 0xFF8 0xFFF, third entry is cluster end of readme file
  },

  //------------- Block2: Root Directory -------------//
  {
    // first entry is volume label
    'A' , 'T' , 'O' , 'M' , 'S' , '3' , 'U' , '_',
    'M' , 'S' , 'C' , 
    0x08, //FILE_ATTR_VOLUME_LABEL
    0x00, 
    FAT_MS2B(0,0), 
    FAT_HMS2B(0,0,0),
    FAT_YMD2B(0,0,0), 
    FAT_YMD2B(0,0,0), 
    FAT_U16(0), 
    FAT_HMS2B(13,42,30),  //last_modified_hms
    FAT_YMD2B(2018,11,5), //last_modified_ymd
    FAT_U16(0), 
    FAT_U32(0),
    
    // second entry is readme file
    'S' , 'E' , 'T' , 'T' , 'I' , 'N' , 'G' , 'S' ,//file_name[8]; padded with spaces (0x20)
    'T' , 'X' , 'T' ,     //file_extension[3]; padded with spaces (0x20)
    0x20,                 //file attributes: FILE_ATTR_ARCHIVE
    0x00,                 //ignore
    FAT_MS2B(1,980),      //creation_time_10_ms (max 199x10 = 1s 990ms)
    FAT_HMS2B(13,42,36),  //create_time_hms [5:6:5] => h:m:(s/2)
    FAT_YMD2B(2018,11,5), //create_time_ymd [7:4:5] => (y+1980):m:d
    FAT_YMD2B(2020,11,5), //last_access_ymd
    FAT_U16(0),           //extended_attributes
    FAT_HMS2B(13,44,16),  //last_modified_hms
    FAT_YMD2B(2019,11,5), //last_modified_ymd
    FAT_U16(2),           //start of file in cluster
    FAT_U32(sizeof(DEFAULT_CONTENTS) - 1) //file size
  },

  //------------- Block3: Readme Content -------------//
  DEFAULT_CONTENTS
};

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
  // void *buf1　：　コピー先のメモリブロック
  // const void *buf2　：　コピー元のメモリブロック
  // size_t n　：　コピーバイト数
  // このmemcpyは動いていない？ この行が無くてもREADME.txtは表示されるし、データの更新もできる(電源が入ってる限り)
  memcpy(msc_disk[lba] + offset, buffer, bufsize);

  flickLed(2, "red");


  writeFlg = 1;
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize){
  USBSerial.printf("1MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
  USBSerial.printf("2MSC READ buffer: %u\n", buffer);

  // memcpy(buf1, buf2, n)
  // void *buf1　：　コピー先のメモリブロック
  // const void *buf2　：　コピー元のメモリブロック
  // size_t n　：　コピーバイト数
  // このmemcpyは動いている この行が無いとREADME.txtファイルもストレージも表示されない
  memcpy(buffer, msc_disk[lba] + offset, bufsize);

  flickLed(2, "yellow");

  readFlg = 1;

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
  USBSerial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);

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


char * initialContents = R"({"color": "red"})";
String fileName = "/ATOMS3U/settings.txt";
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

// bootmode
// 0: Normal
// 1: Settings Mode(USB Flash)
int bootmode = 0;
DynamicJsonDocument settingsDoc(512);


// モード 0: ほぼ動かない
// モード 1: ランダム移動
// モード 2: wheel上下
int mode = 0;

int inputWait = 50;
int loopWait = 10;

int dist = 10;
int wheel_dist = 3;

bool isAction = false;

USBHIDMouse Mouse;


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  auto cfg = M5.config();
  M5.begin(cfg);
  rgbled = new EspEasyLED(GPIO_NUM_35, 1, 20);
  M5.Power.setLed(0);


  if (M5.BtnA.wasPressed()) {
    bootmode = 1;
  }

  // 0. ROM領域の初期化 
  if(SPIFFS.begin()){
    File dataFile;
    // removeAllFiles();
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

  if(bootmode == 0){// 1. normal モード
    USBSerial.begin();
    Mouse.begin();
    USB.begin();
  }else if(bootmode == 1){// 2. USB Flash モード
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

  if(bootmode == 0){// 1. normal モード
    if (M5.BtnA.wasPressed()) {
      isAction = !isAction;
      switch(mode) {
        case 0:
          mode = 1;
          break;
        case 1:
          mode = 2;
          break;
        case 2:
          mode = 0;
          break;
        default:
          mode = 0;
          break;
      }
    }
    
  

    if (mode == 1){
      if (rgbled) {
        rgbled->showColor(255, 0, 255);//magenta
      }
      int rdir = random(0,4);
      int rDist = random(1,dist);
      

      switch (rdir) {
        case 0:
          Mouse.move(0, rDist*-1, 0);
          delay(inputWait);
          break;
        case 1:
          Mouse.move(rDist, 0, 0);
          delay(inputWait);
          break;
        case 2:
          Mouse.move(0, rDist, 0);
          delay(inputWait);
          break;
        case 3:
          Mouse.move(rDist*-1, 0, 0);
          delay(inputWait);
          break;
      }
    }else if (mode == 2){
      if (rgbled) {
          rgbled->showColor(0, 155, 225);
      }

      delay(inputWait);
      Mouse.move(0, 0, wheel_dist);
      delay(inputWait);
      Mouse.move(0, 0, -1 * wheel_dist);
      delay(inputWait);
      Mouse.move(0, 0, wheel_dist);
      delay(inputWait);
      Mouse.move(0, 0, -1 * wheel_dist);

    } else {
      if (rgbled) {
        rgbled->showColor(EspEasyLEDColor::WHITE);
      }

      Mouse.move(1, 0, 0);
      delay(inputWait);
      Mouse.move(0, 1, 0);
      delay(inputWait);
      Mouse.move(-1, 0, 0);
      delay(inputWait);
      Mouse.move(0, -1, 0);
      delay(inputWait);
    }

    delay(loopWait);

  }else if(bootmode == 1){// 2. USB Flash モード
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
  }


}
#endif /* ARDUINO_USB_MODE */
