
#include <NTPClient.h>
#include <WiFiUdp.h>


#include "configuration.h"
#include "mqtt.h"
#include "temperature_and_humidity_sensor.h"
#include "wifi_client.h"

#include "SSD1306.h"

SSD1306 display(0x3c, D3, D5);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void MQTT_connect(void);


void setup() {
  Serial.begin(9600);
  delay(10);

  th_sensor::initialice_sensor();
  wifi::connect();
  timeClient.begin();

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void loop() {
  mqtt_connect();
  timeClient.update();

  float h = th_sensor::dht.readHumidity();
  float t = th_sensor::dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    display.clear();
    display.drawString(5,0, "Failed DHT");
    return;
  }

  float hic = th_sensor::dht.computeHeatIndex(t, h, false);

  display.clear();
  display.drawString(0, 0, "Humidity: " + String(h) + "%\t");
  display.drawString(0, 16, "Temp: " + String(t) + "C");
  display.drawString(0, 32, "Timestamp: " + timeClient.getEpochTime());


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
