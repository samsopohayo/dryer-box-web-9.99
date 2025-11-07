#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include "../globals.h"

class DisplayManager {
public:
  static void init();
  static void update();
  static void updateDefault();
  static void updateDoorStatus();
  static void cycleInfo();
  static void handleInfoTimeout();
  
private:
  static void displayBoot();
  static void displayInfo();
  static void displayErrors();
};

#endif 