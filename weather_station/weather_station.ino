
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


#include "configuration.h"
#include "mqtt.h"
#include "temperature_and_humidity_sensor.h"
#include "wifi_client.h"

typedef union{
  struct __attribute__((__packed__)){  // packed to eliminate padding for easier parsing.
    unsigned long timestamp;
    float value;
  }data;

  uint8_t raw[sizeof(data)];                    // For publishing
} meassure_packet;

meassure_packet humidity_queue[QUEUES_SIZE];
meassure_packet temperature_queue[QUEUES_SIZE];
int queue_index = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void MQTT_connect(void);


void setup() {
  Serial.begin(9600);
  delay(10);

  th_sensor::initialice_sensor();
  wifi::connect();
  timeClient.begin();

 // Check connection
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin("http://grigri.grigri/");
  Serial.print("[HTTP] GET...\n");

  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();


}

void loop() {
  mqtt_connect();

  timeClient.update();

  unsigned long timestamp = timeClient.getEpochTime();

  humidity_queue[queue_index].data.timestamp = timestamp;
  temperature_queue[queue_index].data.timestamp = timestamp;

  humidity_queue[queue_index].data.value = th_sensor::dht.readHumidity();
  temperature_queue[queue_index].data.value = th_sensor::dht.readTemperature();

  if (isnan(humidity_queue[queue_index].data.value) || isnan(temperature_queue[queue_index].data.value)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //float hic = th_sensor::dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(humidity_queue[queue_index].data.value);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature_queue[queue_index].data.value);
  Serial.print(" *C\t");

  Serial.print("timestamp: ");
  Serial.println(temperature_queue[queue_index].data.timestamp);
  Serial.println();

  const char* topic = "/weather-station/indoor/1";
  Adafruit_MQTT_Publish* weather_station_indoor_1;
  weather_station_indoor_1 = get_mqtt_publisher(topic);
  Serial.println("\nSending weather-station values");
  if (! weather_station_indoor_1->publish(humidity_queue[queue_index].raw, sizeof(meassure_packet))) {
    //TODO: store in buffer and send in next try
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  delay(2000);
}
