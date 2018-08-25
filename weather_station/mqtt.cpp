
#include "mqtt.h"

#define QOS 1
#define IS_CONNECTED 0

extern Adafruit_MQTT_Client mqtt(&wifi::client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Adafruit_MQTT_Publish* get_mqtt_publisher(const char* topic){
    Serial.print("Topic publisher: ");
    Serial.println(topic);
    Adafruit_MQTT_Publish* publisher = new Adafruit_MQTT_Publish(&mqtt, topic, QOS);
    return publisher;
}

void mqtt_connect(void) {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != IS_CONNECTED) {
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
