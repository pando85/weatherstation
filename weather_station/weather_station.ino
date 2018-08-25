
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


#include "configuration.h"
#include "mqtt.h"
#include "temperature_and_humidity_sensor.h"
#include "wifi_client.h"

struct _data{
  unsigned long timestamp;
  float value;
};

char* get_json_from_data(struct _data data){
  StaticJsonBuffer<200> jsonBuffer;
  char json[40];
  JsonObject& root = jsonBuffer.createObject();
  root["timestamp"] = data.timestamp;
  root.set<float>("value", data.value);
  root.printTo(json);
  return json;
}

struct buffer{
  const char* topic;
  int queue_index = 0;
  struct _data data[QUEUES_SIZE];
};

struct buffer humidity_buffer;
struct buffer temperature_buffer;

Adafruit_MQTT_Publish* weather_station_indoor_1_humidity;
Adafruit_MQTT_Publish* weather_station_indoor_1_temperature;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void MQTT_connect(void);


void setup() {
  Serial.begin(9600);
  delay(10);

  th_sensor::initialice_sensor();
  wifi::connect();
  timeClient.begin();
  humidity_buffer.topic = "/weather-station/indoor/1/humidity";
  temperature_buffer.topic = "/weather-station/indoor/1/temperature";
  weather_station_indoor_1_humidity = get_mqtt_publisher(humidity_buffer.topic);
  weather_station_indoor_1_temperature = get_mqtt_publisher(temperature_buffer.topic);
}

void loop() {
  mqtt_connect();

  timeClient.update();

  unsigned long timestamp = timeClient.getEpochTime();

  humidity_buffer.data[humidity_buffer.queue_index].timestamp = timestamp;
  humidity_buffer.data[humidity_buffer.queue_index].value = th_sensor::dht.readHumidity();
  temperature_buffer.data[temperature_buffer.queue_index].timestamp = timestamp;
  temperature_buffer.data[temperature_buffer.queue_index].value = th_sensor::dht.readTemperature();

  if (isnan(humidity_buffer.data[humidity_buffer.queue_index].value) || isnan(temperature_buffer.data[temperature_buffer.queue_index].value)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //float hic = th_sensor::dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(humidity_buffer.data[humidity_buffer.queue_index].value);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature_buffer.data[temperature_buffer.queue_index].value);
  Serial.print(" *C\t");
  Serial.print("timestamp: ");
  Serial.println(temperature_buffer.data[temperature_buffer.queue_index].timestamp);
  Serial.println();

  Serial.println("\nSending weather-station values");

  bool is_success = true;
  while (is_success == true && temperature_buffer.data[temperature_buffer.queue_index].value != NULL){
    char data_json[40];
    strcpy(data_json, get_json_from_data(temperature_buffer.data[temperature_buffer.queue_index]));

    if (! weather_station_indoor_1_temperature->publish(data_json)) {
      temperature_buffer.queue_index += 1;
      if (temperature_buffer.queue_index >= QUEUES_SIZE){
        temperature_buffer.queue_index = 0;
      }
      is_success = false;
      Serial.print("Buffer index: ");
      Serial.println(temperature_buffer.queue_index);

      Serial.println(F("Failed to send!"));
    } else {
      temperature_buffer.data[temperature_buffer.queue_index].value = NULL;
      Serial.println(F("Succesfully sent!"));
    }
  }

  delay(2000);
}
