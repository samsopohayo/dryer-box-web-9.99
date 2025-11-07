#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include "../globals.h"

class SensorManager {
public:
  static void init();
  static void readAll();
  static void testDHT();
  static void testLoadCell();
  static void checkObjectWeight();
  
private:
  static void readDHT();
  static void readLoadCell();
};

#endif