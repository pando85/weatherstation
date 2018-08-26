
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


#include "configuration.h"
#include "data_structure.h"
#include "mqtt.h"
#include "temperature_and_humidity_sensor.h"
#include "wifi_client.h"


_th_sensor_data th_sensor_data = {
  .temperature = {
    .topic = MQTT_TEMPERATURE_TOPIC,
    .queue_index = 0
  },
  .humidity = {
    .topic = MQTT_HUMIDITY_TOPIC,
    .queue_index = 0
  }
};

Adafruit_MQTT_Publish* weather_station_indoor_1_humidity;
Adafruit_MQTT_Publish* weather_station_indoor_1_temperature;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


void setup() {
  Serial.begin(9600);
  delay(10);

  th_sensor::initialice_sensor();
  wifi::connect();
  timeClient.begin();

  weather_station_indoor_1_humidity = get_mqtt_publisher(th_sensor_data.humidity.topic);
  weather_station_indoor_1_temperature = get_mqtt_publisher(th_sensor_data.temperature.topic);
}

void loop() {
  mqtt_connect();

  timeClient.update();

  unsigned long timestamp = timeClient.getEpochTime();

  th_sensor_data.humidity.data[th_sensor_data.humidity.queue_index].timestamp = timestamp;
  th_sensor_data.humidity.data[th_sensor_data.humidity.queue_index].value = th_sensor::dht.readHumidity();
  th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].timestamp = timestamp;
  th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value = th_sensor::dht.readTemperature();

  if (isnan(th_sensor_data.humidity.data[th_sensor_data.humidity.queue_index].value) || isnan(th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //float hic = th_sensor::dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(th_sensor_data.humidity.data[th_sensor_data.humidity.queue_index].value);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value);
  Serial.print(" *C\t");
  Serial.print("timestamp: ");
  Serial.println(th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].timestamp);
  Serial.println();

  Serial.println("\nSending weather-station values");

  bool is_success = true;
  while (is_success == true && th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value != NULL){
    char data_json[40];
    strcpy(data_json, get_json_from_data(th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index]));

    if (! weather_station_indoor_1_temperature->publish(data_json)) {
      th_sensor_data.temperature.queue_index += 1;
      if (th_sensor_data.temperature.queue_index >= QUEUES_SIZE){
        th_sensor_data.temperature.queue_index = 0;
      }
      is_success = false;
      Serial.print("Buffer index: ");
      Serial.println(th_sensor_data.temperature.queue_index);

      Serial.println(F("Failed to send!"));
    } else {
      th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value = NULL;
      Serial.println(F("Succesfully sent!"));
    }
  }

  delay(2000);
}
