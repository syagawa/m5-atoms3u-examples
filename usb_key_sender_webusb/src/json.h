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

DynamicJsonDocument pushValueByKeyToJson(DynamicJsonDocument doc, String key, String value){
  JsonObject obj = doc.as<JsonObject>();
  JsonArray array;

  if (obj.containsKey(key)) {
    array = obj[key].as<JsonArray>();
  } else {
    array = obj.createNestedArray(key);
  }

  array.add(value);

  DynamicJsonDocument dynamicDoc(512);
  for (JsonPair kv : obj) {
    dynamicDoc[kv.key()] = kv.value();
  }
  return dynamicDoc;
}

DynamicJsonDocument popValueByKeyToJson(DynamicJsonDocument doc, String key){
  JsonObject obj = doc.as<JsonObject>();
  JsonArray array;

  if (obj.containsKey(key)) {
    array = obj[key].as<JsonArray>();

    if (!array.isNull() && array.size() > 0) {
      array.remove(array.size() - 1);
      obj[key] = array;
    }
  }

  DynamicJsonDocument dynamicDoc(512);
  for (JsonPair kv : obj) {
    dynamicDoc[kv.key()] = kv.value();
  }
  return dynamicDoc;
}

DynamicJsonDocument removeKeyValueInJson(DynamicJsonDocument doc, String key){
  JsonObject obj = doc.as<JsonObject>();
  obj.remove(key);

  DynamicJsonDocument dynamicDoc(512);

  for (JsonPair kv : obj) {
    dynamicDoc[kv.key()] = kv.value();
  }

  return dynamicDoc;
}

#endif //JSON_H
