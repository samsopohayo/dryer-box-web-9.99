#ifndef FIREBASEMANAGER_H
#define FIREBASEMANAGER_H

#include "../globals.h"

class FirebaseManager {
public:
  static void setup();
  static void loadConfig();
  static void listenConfigUpdates();
  static void sendSystemStatus(String status);
  static void sendSystemInfo();
  static void sendSessionStart();
  static void sendSessionEnd();
  static void sendData();
  
private:
  static void sendSensorData();
  static void sendStatusData();
  static void sendSessionData();
};

#endif 