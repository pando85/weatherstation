
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

  publish_all(&th_sensor_data, &publisher);
  delay(MIN_SENSOR_READ_TIME);
}
