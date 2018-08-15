
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


#include "configuration.h"
#include "mqtt.h"
#include "temperature_and_humidity_sensor.h"
#include "wifi_client.h"



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

  float h = th_sensor::dht.readHumidity();
  float t = th_sensor::dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hic = th_sensor::dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.println(" *C");

  Serial.print("timestamp: ");
  unsigned long timestamp = timeClient.getEpochTime();
  Serial.println(timestamp);
  Serial.println();

  const char* topic = "/weather-station/indoor/1";
  Adafruit_MQTT_Publish* weather_station_indoor_1;
  weather_station_indoor_1 = get_mqtt_publisher(topic);
  Serial.println("\nSending weather-station values");
  if (! weather_station_indoor_1->publish(h)) {
    //TODO: store in buffer and send in next try
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  delay(2000);
}
