
#include "mqtt.h"

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.

extern Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

extern Adafruit_MQTT_Publish weather_station_indoor_1(&mqtt, "/weather-station/indoor/1", 1);
