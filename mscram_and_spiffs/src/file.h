#ifndef FILE_H
#define FILE_H

#include <SPIFFS.h>

String fileName = "/ATOMS3U/SETTINGS.TXT";

void writeToFile(String str){
  File file = SPIFFS.open(fileName, "w");
  file.print(str);
  file.close();
}

void listAllFiles(){
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
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


File initAndGetDataFile(char * initialContents, String filename){
  File dataFile;
  if(SPIFFS.begin()){
    if(!SPIFFS.exists(filename)){
      dataFile = SPIFFS.open(filename, "w");
      dataFile.print(initialContents);
      dataFile.close();
    }
    dataFile = SPIFFS.open(filename, "r");
  }
  return dataFile;
}
#endif //FILE_H
