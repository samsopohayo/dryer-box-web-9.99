#include "ErrorManager.h"
#include "../network/FirebaseManager.h"
#include "../utils/TimeUtils.h"

void ErrorManager::init() {
  for (int i = 0; i < 10; i++) {
    systemErrors[i].active = false;
  }
  errorCount = 0;
  hasActiveErrors = false;
  Serial.println("✅ Error manager initialized");
}

void ErrorManager::addError(String code, String message) {
  // Check if error already exists
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].code == code && systemErrors[i].active) {
      return; // Error already exists
    }
  }
  
  // Add new error
  if (errorCount < 10) {
    systemErrors[errorCount].code = code;
    systemErrors[errorCount].message = message;
    systemErrors[errorCount].active = true;
    systemErrors[errorCount].timestamp = millis();
    errorCount++;
    hasActiveErrors = true;
    
    Serial.println("❌ ERROR ADDED: [" + code + "] " + message);
    
    // Send error to Firebase
    if (WiFi.status() == WL_CONNECTED) {
      Firebase.setString(firebaseData, "/system/last_error", code + ": " + message);
      Firebase.setString(firebaseData, "/system/error_time", TimeUtils::getTimestamp());
    }
  }
}

void ErrorManager::clearError(String code) {
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].code == code && systemErrors[i].active) {
      systemErrors[i].active = false;
      Serial.println("✅ ERROR CLEARED: [" + code + "]");
      break;
    }
  }
  
  // Check if any errors still active
  hasActiveErrors = false;
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].active) {
      hasActiveErrors = true;
      break;
    }
  }
  
  // Clear Firebase error if no more active errors
  if (!hasActiveErrors && WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/last_error", "No active errors");
  }
}

void ErrorManager::clearAllErrors() {
  for (int i = 0; i < errorCount; i++) {
    systemErrors[i].active = false;
  }
  hasActiveErrors = false;
  errorCount = 0;
  Serial.println("✅ All errors cleared");
  
  // Clear Firebase errors
  if (WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/last_error", "All errors cleared");
    Firebase.setString(firebaseData, "/system/error_time", TimeUtils::getTimestamp());
  }
}

bool ErrorManager::hasErrors() {
  return hasActiveErrors;
}

String ErrorManager::getFirstActiveError() {
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].active) {
      return systemErrors[i].code;
    }
  }
  return "";
}