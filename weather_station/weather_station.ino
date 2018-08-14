
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

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


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

  timeClient.begin();

}

void loop() {
  MQTT_connect();

  // Reading temperature or humidity takes about 250 milliseconds!
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

  timeClient.update();


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


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
