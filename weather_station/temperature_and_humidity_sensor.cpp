
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

} // th_sensor
