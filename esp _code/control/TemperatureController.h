#ifndef TEMPERATURECONTROLLER_H
#define TEMPERATURECONTROLLER_H

#include "../globals.h"

class TemperatureController {
public:
  static void checkProtection();
  static void checkControl();
  
private:
  static void controlExhaust(float temperature);
};

#endif