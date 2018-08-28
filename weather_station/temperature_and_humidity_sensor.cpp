
#include "temperature_and_humidity_sensor.h"

namespace th_sensor
{

  extern DHT dht(DHT_OUT_PIN, DHTTYPE);

  extern void initialice_sensor(void){
      pinMode(DHT_VCC_PIN, OUTPUT);
      _power_on();

      dht.begin();
  }

  void _power_on(void){
      digitalWrite(DHT_VCC_PIN, HIGH);

  }

  extern bool get_data(_th_sensor_data* th_sensor_data,const unsigned long timestamp){

    th_sensor_data->humidity.data[th_sensor_data->humidity.queue_index].timestamp = timestamp;
    th_sensor_data->humidity.data[th_sensor_data->humidity.queue_index].value = th_sensor::dht.readHumidity();
    th_sensor_data->temperature.data[th_sensor_data->temperature.queue_index].timestamp = timestamp;
    th_sensor_data->temperature.data[th_sensor_data->temperature.queue_index].value = th_sensor::dht.readTemperature();

    if (isnan(th_sensor_data->humidity.data[th_sensor_data->humidity.queue_index].value) || isnan(th_sensor_data->temperature.data[th_sensor_data->temperature.queue_index].value)) {
      Serial.println("Failed to read from DHT sensor!");
      return false;
    }

    Serial.print("Temperature: ");
    Serial.print(th_sensor_data->temperature.data[th_sensor_data->temperature.queue_index].value);
    Serial.print(" *C\t");
    Serial.print("Humidity: ");
    Serial.print(th_sensor_data->humidity.data[th_sensor_data->humidity.queue_index].value);
    Serial.print(" %\t");
    Serial.print("timestamp: ");
    Serial.println(th_sensor_data->temperature.data[th_sensor_data->temperature.queue_index].timestamp);
    Serial.println();

    return true;
  }

} // th_sensor
