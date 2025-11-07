#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include "../globals.h"

class ButtonHandler {
public:
  static void handleAll();
  static void handleTareButton();
  static void handleStartButton();
  static void handleInfoButton();
  static void testPins();
  
private:
  static void performTare();
  static void restartSystem();
};

#endif 