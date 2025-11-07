#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include "../globals.h"

class ErrorManager {
public:
  static void init();
  static void addError(String code, String message);
  static void clearError(String code);
  static void clearAllErrors();
  static bool hasErrors();
  static String getFirstActiveError();
};

#endif