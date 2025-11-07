#include "StateManager.h"
#include "../sensors/SensorManager.h"
#include "../control/FuzzyController.h"
#include "../control/ManualController.h"
#include "../network/FirebaseManager.h"
#include "../storage/OfflineStorage.h"
#include "../utils/TimeUtils.h"
#include "../utils/Calculations.h"

// Forward declaration untuk startDryingProcess yang dipanggil dari ButtonHandler
void startDryingProcess() {
  Serial.println("\n🚀 === STARTING DRYING PROCESS ===");
  
  sessionId = TimeUtils::generateSessionId();
  Serial.println("   Session ID: " + sessionId);
  Serial.println("   Initial weight: " + String(beratAwal, 1) + " gram");
  Serial.println("   Humidity target: " + String(targetHumidityMin, 1) + "-" + String(targetHumidityMax, 1) + "%");
  Serial.println("   Temperature target: " + String(targetTemperatureMin, 1) + "-" + String(targetTemperatureMax, 1) + "°C");

  systemRunning = true;
  currentState = RUNNING;
  processStatus = "Berjalan";
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PROSES DIMULAI!");
  delay(2000);
  
  FirebaseManager::sendSessionStart();
  
  Serial.println("✅ Drying process started successfully!");
}

void StateManager::run() {
  switch (currentState) {
    case WAIT_TARE:
      break;
      
    case WAIT_OBJECT:
      SensorManager::checkObjectWeight();
      break;
      
    case WAIT_START:
      break;
      
    case RUNNING:
      if (systemRunning) {
        runDryingProcess();
        delay(15000);
      }
      break;
  }
}

void StateManager::runDryingProcess() {
  Serial.println("\n📊 === DRYING PROCESS CYCLE ===");
  
  SensorManager::readAll();
  
  kadarAir = Calculations::calculateMoistureContent(beratSekarang, beratAwal);
  Serial.println("📊 Moisture content: " + String(kadarAir, 1) + "%");
  
  checkProcessCompletion();
  
  if (processStatus == "Berjalan" && !tempProtectionActive && !doorOpen) {
    if (manualHeaterControl || manualFanColControl || manualFanPanControl || manualExhaustControl) {
      ManualController::apply();
    } else {
      FuzzyController::apply();
    }
  } else {
    extern void turnOffAllOutputs();
    turnOffAllOutputs();
  }
  
  OfflineStorage::save();
  
  if (WiFi.status() == WL_CONNECTED && !offlineMode) {
    FirebaseManager::sendData();
  } else {
    Serial.println("📤 Data buffered offline");
  }
  
  Serial.println("✅ Process cycle completed");
}

void StateManager::checkProcessCompletion() {
  if (kadarAir <= targetMoisture && processStatus == "Berjalan") {
    Serial.println("🎉 Drying process completed!");
    Serial.println("   Target moisture reached: " + String(kadarAir, 1) + "%");
    
    processStatus = "Selesai";
    systemRunning = false;
    humidityControlActive = false;
    temperatureControlActive = false;
    
    extern void turnOffAllOutputs();
    turnOffAllOutputs();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PROSES SELESAI!");
    lcd.setCursor(0, 1);
    lcd.print("KA:" + String(kadarAir, 0) + "% H:" + String(kelembapanSensor, 0) + "%");
    
    FirebaseManager::sendSessionEnd();
  }
}