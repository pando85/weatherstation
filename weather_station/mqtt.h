
#ifndef MQTT_H
#define MQTT_H

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "configuration.h"
#include "wifi_client.h"

extern Adafruit_MQTT_Client mqtt;

Adafruit_MQTT_Publish* get_mqtt_publisher(const char* topic);

#endif // MQTT_H