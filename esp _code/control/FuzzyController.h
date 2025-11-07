#ifndef FUZZYCONTROLLER_H
#define FUZZYCONTROLLER_H

#include "../globals.h"

class FuzzyController {
public:
  static void setup();
  static void apply();
  static void controlHeater(float output);
  static void controlFan(float output);
  static void controlExhaust(float output);
  
private:
  static void setupInputsOutputs();
  static void setupRules();
};

#endif