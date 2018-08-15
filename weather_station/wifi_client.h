
#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <ESP8266WiFi.h>

#include "configuration.h"

namespace wifi
{

  extern WiFiClient client;

  void connect(void);

} // wifi
#endif // WIFI_CLIENT_H
