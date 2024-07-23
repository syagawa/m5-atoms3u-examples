#ifndef JSON_H
#define JSON_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

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

DynamicJsonDocument getJsonDocumentFromFile(String fileName){
  DynamicJsonDocument doc(512);
  try{
    File file = SPIFFS.open(fileName, FILE_READ);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if(error){
      // USBSerial.printf("doc file error %s \n", error.c_str());
    }
  }catch(Exception& ex){
    // USBSerial.printf("json error %s \n", ex.Message());
  }
  return doc;
}

String getJsonString(DynamicJsonDocument doc){
  String output;
  serializeJson(doc, output);
  return output;
}


DynamicJsonDocument setKeyValueToJson(DynamicJsonDocument doc, String key, String value){
  doc[key] = value;
  return doc;
}

DynamicJsonDocument removeKeyValueInJson(DynamicJsonDocument doc, String key){
  doc.remove(key);
  return doc;
}

#endif //JSON_H
