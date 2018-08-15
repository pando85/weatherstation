
#include "mqtt.h"

extern Adafruit_MQTT_Client mqtt(&wifi::client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Adafruit_MQTT_Publish* get_mqtt_publisher(const char* topic){
    Adafruit_MQTT_Publish* weather_station_indoor_1 = new Adafruit_MQTT_Publish(&mqtt, topic, 1);
    return weather_station_indoor_1;
}

void mqtt_connect(void) {
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
