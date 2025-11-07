#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "../globals.h"

class StateManager {
public:
  static void run();
  
private:
  static void runDryingProcess();
  static void checkProcessCompletion();
};

#endif 