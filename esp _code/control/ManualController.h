#ifndef MANUALCONTROLLER_H
#define MANUALCONTROLLER_H

#include "../globals.h"

class ManualController {
public:
  static void check();
  static void apply();
  static void controlHeater(bool state);
  static void controlFanPanel(bool state);
  static void controlFanCollector(bool state);
  static void controlExhaust(bool state);
};

#endif