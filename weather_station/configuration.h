
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define DHTTYPE DHT22

#define DHT_VCC_PIN 4
#define DHT_OUT_PIN 5

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "super_secret_password"

#define QUEUES_SIZE 10

#define MQTT_STATION_TOPIC "/weather-station/indoor/1"
#define MQTT_TEMPERATURE_TOPIC MQTT_STATION_TOPIC "/temperature"
#define MQTT_HUMIDITY_TOPIC MQTT_STATION_TOPIC "/humidity"

#define AIO_SERVER "example.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "user1234"
#define AIO_KEY "password1234"

#endif // CONFIGURATION_H
