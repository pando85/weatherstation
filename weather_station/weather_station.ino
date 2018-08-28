
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

mqtt_th_sensor_publisher publisher;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


void setup() {
  Serial.begin(9600);
  delay(10);

  th_sensor::initialice_sensor();
  wifi::connect();
  timeClient.begin();

  publisher.temperature = get_mqtt_publisher(th_sensor_data.temperature.topic);
  publisher.humidity = get_mqtt_publisher(th_sensor_data.humidity.topic);
}

void loop() {
  mqtt_connect();

  timeClient.update();

  unsigned long timestamp = timeClient.getEpochTime();

  if (! th_sensor::get_data(&th_sensor_data, timestamp)){
    return;
  }

  Serial.println("\nSending weather-station values");
  bool is_success = true;
  while (is_success == true && th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index].value != NULL){
    char data_json[40];
    strcpy(data_json, get_json_from_data(th_sensor_data.temperature.data[th_sensor_data.temperature.queue_index]));

    if (! publisher.temperature->publish(data_json)) {
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
