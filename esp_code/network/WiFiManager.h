#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include "../globals.h"

class WiFiManager {
public:
  static void connect();
  static bool isConnected();
  static int getRSSI();
};

#endif