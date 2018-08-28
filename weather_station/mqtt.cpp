
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

bool mqtt_connect(void) {
  int8_t ret;

  if (mqtt.connected()) {
    return true;
  }

  Serial.print("Connecting to MQTT... ");

  if (mqtt.connect() != IS_CONNECTED){
    Serial.println("Retrying MQTT connection in 2 seconds...");
    Serial.println(mqtt.connectErrorString(ret));
    mqtt.disconnect();
    return false;
  }
  Serial.println("MQTT Connected!");
  return true;
}


extern void publish_all(_th_sensor_data* th_sensor_data, mqtt_th_sensor_publisher* th_sensor_publisher){
  Serial.println("\nSending weather-station values");
  publish_data(&(th_sensor_data->temperature), th_sensor_publisher->temperature);
  publish_data(&(th_sensor_data->humidity), th_sensor_publisher->humidity);
}


void publish_data(buffer* buffer, Adafruit_MQTT_Publish* publisher){
  while (true){
    char data_json[40];
    strcpy(data_json, get_json_from_data(buffer->data[buffer->queue_index]));

    Serial.print("Buffer index: ");
    Serial.println(buffer->queue_index);
    if (!publisher->publish(data_json)) {
      buffer->queue_index += 1;
      if (buffer->queue_index >= QUEUES_SIZE){
        buffer->queue_index = 0;
      }
      Serial.println(F("Failed to send!"));
      break;
    }

    buffer->queue_index -= 1;
    if (buffer->queue_index < 0){
      buffer->queue_index = 0;
      break;
    }
  }

}

// enviar hasta fallar o mandar todos los datos
// si se envia
// enviar siguiente dato POP
// si no se envia
// a coger más datos -> cuando read hacer dato PUSH

// mejor que FIFO un buffer con un mecanismo de interpolacion por timestamp para
// perder la menos informacion posible de la muestra
// tiene que saber la longitud, el inicio y el final
