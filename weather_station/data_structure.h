#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include "configuration.h"


typedef struct _data{
  unsigned long timestamp;
  float value;
};

typedef struct buffer{
  const char* topic;
  int queue_index;
  _data data[QUEUES_SIZE];
};

typedef struct _th_sensor_data{
  buffer temperature;
  buffer humidity;
};


char* get_json_from_data(struct _data data);

#endif // DATA_STRUCTURE_H
