
#ifndef MQTT_H
#define MQTT_H

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "configuration.h"
#include "data_structure.h"
#include "wifi_client.h"

extern Adafruit_MQTT_Client mqtt;

Adafruit_MQTT_Publish* get_mqtt_publisher(const char* topic);
void mqtt_connect(void);
extern void publish_all(_th_sensor_data*, mqtt_th_sensor_publisher*);
void publish_data(buffer*, Adafruit_MQTT_Publish*);
#endif // MQTT_H