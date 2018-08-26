
#include <ArduinoJson.h>

#include "data_structure.h"

char* get_json_from_data(struct _data data){
  StaticJsonBuffer<200> jsonBuffer;
  char json[40];
  JsonObject& root = jsonBuffer.createObject();
  root["timestamp"] = data.timestamp;
  root.set<float>("value", data.value);
  root.printTo(json);
  return json;
}
