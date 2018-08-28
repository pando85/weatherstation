
#ifndef TEMPERATURE_AND_HUMIDITY_SENSOR_H
#define TEMPERATURE_AND_HUMIDITY_SENSOR_H

#include <DHT.h>

#include "configuration.h"
#include "data_structure.h"


namespace th_sensor
{
  extern DHT dht;

  extern void initialice_sensor(void);

  void _power_on(void);

  extern bool get_data(_th_sensor_data*, const unsigned long);

} // th_sensor
#endif // TEMPERATURE_AND_HUMIDITY_SENSOR_H