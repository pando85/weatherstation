
#ifndef TEMPERATURE_AND_HUMIDITY_SENSOR_H
#define TEMPERATURE_AND_HUMIDITY_SENSOR_H

#include <DHT.h>

#include "configuration.h"


namespace th_sensor
{
  extern DHT dht;

  extern void initialice_sensor(void);

  void _power_on(void);

} // th_sensor
#endif // TEMPERATURE_AND_HUMIDITY_SENSOR_H