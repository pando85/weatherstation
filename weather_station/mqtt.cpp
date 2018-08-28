
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
    delay(5000);
    retries--;
    if (retries == 0) {
      exit (1);
    }
  }
  Serial.println("MQTT Connected!");
}


extern void publish_all(_th_sensor_data* th_sensor_data, mqtt_th_sensor_publisher* th_sensor_publisher){
  publish_data(&(th_sensor_data->temperature), th_sensor_publisher->temperature);
  publish_data(&(th_sensor_data->humidity), th_sensor_publisher->humidity);
}


void publish_data(buffer* buffer, Adafruit_MQTT_Publish* publisher){
  bool is_success = true;
  while (is_success == true && buffer->data[buffer->queue_index].value != NULL){
    char data_json[40];
    strcpy(data_json, get_json_from_data(buffer->data[buffer->queue_index]));

    if (! publisher->publish(data_json)) {
      buffer->queue_index += 1;
      if (buffer->queue_index >= QUEUES_SIZE){
        buffer->queue_index = 0;
      }
      is_success = false;
      Serial.print("Buffer index: ");
      Serial.println(buffer->queue_index);

      Serial.println(F("Failed to send!"));
    } else {
      buffer->data[buffer->queue_index].value = NULL;
      Serial.println(F("Succesfully sent!"));
    }
  }
}
