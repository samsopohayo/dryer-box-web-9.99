#ifndef HUMIDITYCONTROLLER_H
#define HUMIDITYCONTROLLER_H

#include "../globals.h"

class HumidityController {
public:
  static void check();
  
private:
  static void controlExhaust(float humidity);
};

#endif