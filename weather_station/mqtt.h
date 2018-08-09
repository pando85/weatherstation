
#ifndef MQTT_H
#define MQTT_H

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "configuration.h"
#include "wifi_client.h"

extern Adafruit_MQTT_Client mqtt;
extern Adafruit_MQTT_Publish weather_station_indoor_1;

#endif // MQTT_H