#ifndef FILE_H
#define FILE_H

#include <SPIFFS.h>

String fileName = "/ATOMS3U/SETTINGS.TXT";


void writeToFile(String str){
  File file = SPIFFS.open(fileName, FILE_WRITE);
  file.print(str);
  file.close();
}

void listAllFiles(){
  File root = SPIFFS.open("/", FILE_READ);
  File file = root.openNextFile();
  while(file){
    file = root.openNextFile();
  }
}

void removeAllFiles(){
  File root = SPIFFS.open("/", FILE_READ);
  File file = root.openNextFile();
  while(file){
    SPIFFS.remove(file.path());
    file = root.openNextFile();
  }
}

// void overWriteContentsOnMemory( const char *contents){
//   uint8_t * u = (uint8_t *)contents;
//   int len = strlen(contents);
//   for(int i = 0; i < len; i++){
//     msc_disk[3][i] = u[i];
//   }
// }

File initAndGetDataFile(char * initialContents, String filename, bool force){
  File dataFile;
  if(SPIFFS.begin(true)){
    if(!SPIFFS.exists(filename) || force){
      dataFile = SPIFFS.open(filename, FILE_WRITE);
      dataFile.print(initialContents);
      dataFile.close();
    }
    dataFile = SPIFFS.open(filename, FILE_READ);
  }
  return dataFile;
}


void initRomArea(char * initialContents, bool force){

  File dataFile = initAndGetDataFile(initialContents, fileName, force);
  String readStr = dataFile.readString();
  // char Buf[DISK_SECTOR_SIZE];
  // readStr.toCharArray(Buf, DISK_SECTOR_SIZE);
  // initialContents = Buf;
  dataFile.close();
  settingsDocInMain = getJsonDocumentFromFile(fileName);

  // overWriteContentsOnMemory(initialContents);
}

#endif //FILE_H
