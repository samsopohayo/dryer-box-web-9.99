#include "ButtonHandler.h"
#include "../display/DisplayManager.h"
#include "../sensors/SensorManager.h"
#include "../network/FirebaseManager.h"
#include "../system/ErrorManager.h"

void ButtonHandler::handleAll() {
  handleTareButton();
  handleStartButton();
  handleInfoButton();
}

void ButtonHandler::handleTareButton() {
  static bool isPressed = false;
  static unsigned long pressStartTime = 0;

  bool currentReading = (digitalRead(TARE_SWITCH) == LOW);

  if (currentReading && !isPressed) {
    isPressed = true;
    pressStartTime = millis();
    Serial.println("🔘 TARE button pressed - monitoring long press...");
  }
  
  if (isPressed && currentReading) {
    unsigned long heldTime = millis() - pressStartTime;

    if (heldTime >= 3000) {  
      for (int i = 1; i <= 3; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hold for restart");
        lcd.setCursor(0, 1);
        lcd.print("Count: " + String(i));
        Serial.println("⏱️ Hold count: " + String(i));
        delay(1000);
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RESTARTING");
      lcd.setCursor(0, 1);
      lcd.print("SYSTEM...");
      Serial.println("🔁 Long press - Restart system!");
      delay(3000);

      restartSystem();
      isPressed = false;
    }
  }

  if (!currentReading && isPressed) {
    unsigned long heldTime = millis() - pressStartTime;
    isPressed = false;

    if (heldTime < 3000) {
      Serial.println("🔘 Short press - perform TARE");
      performTare();
    }
  }
}

void ButtonHandler::handleStartButton() {
  bool currentReading = (digitalRead(START_SWITCH) == LOW);
  unsigned long currentTime = millis();
  
  if (currentReading && !startButton.lastPressed && 
      (currentTime - startButton.lastPressTime) > 50) {
    
    startButton.lastPressTime = currentTime;
    Serial.println("🔘 START button pressed");
    
    if (currentState == WAIT_START) {
      // Start drying process - will be implemented in StateManager
      extern void startDryingProcess();
      startDryingProcess();
    } else {
      Serial.println("   START ignored - system not ready");
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("START IGNORED");
      lcd.setCursor(0, 1);
      lcd.print("System not ready");
      delay(1500);
    }
  }
  
  startButton.lastPressed = currentReading;
}

void ButtonHandler::handleInfoButton() {
  bool currentReading = (digitalRead(INFO_SWITCH) == LOW);
  unsigned long currentTime = millis();
  
  if (currentReading && !infoButton.lastPressed && 
      (currentTime - infoButton.lastPressTime) > 50) {
    
    infoButton.lastPressTime = currentTime;
    Serial.println("🔘 INFO button pressed");
    
    if (!hasActiveErrors) {
      DisplayManager::cycleInfo();
    } else {
      Serial.println("   INFO ignored - system has active errors");
    }
  }
  
  infoButton.lastPressed = currentReading;
}

void ButtonHandler::performTare() {
  Serial.println("\n⚖  === PERFORMING TARE OPERATION ===");
  
  ErrorManager::clearError("SCALE_NOT_READY");
  ErrorManager::clearError("TARE_FAILED");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Melakukan TARE..");
  lcd.setCursor(0, 1);
  lcd.print("Mohon tunggu...");
  
  if (!scale.is_ready()) {
    ErrorManager::addError("SCALE_NOT_READY", "Scale not ready!");
    Serial.println("❌ Scale not ready for tare!");
    return;
  }
  
  Serial.println("🔄 Taring scale...");
  scale.tare(20);
  delay(2000);
  
  float testReading = scale.get_units(3);
  if (abs(testReading) < 5.0) {
    Serial.println("✅ Tare completed successfully");
    
    currentState = WAIT_OBJECT;
    processStatus = "Siap";
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TARE SUCCESS!");
    lcd.setCursor(0, 1);
    lcd.print("Masukkan objek");
    delay(2000);
    
    FirebaseManager::sendSystemStatus("tare_completed");
    
  } else {
    ErrorManager::addError("TARE_FAILED", "Tare verification failed: " + String(testReading, 2) + "g");
    Serial.println("⚠  Tare verification failed");
  }
}

void ButtonHandler::restartSystem() {
  Serial.println("\n=== LONG PRESS RESTART INITIATED ===");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RESTARTING...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  
  systemRunning = false;
  extern void turnOffAllOutputs();
  turnOffAllOutputs();
  delay(1000);
  
  // Reset all states
  currentState = WAIT_TARE;
  processStatus = "Ready";
  beratAwal = 0;
  beratSekarang = 0;
  kadarAir = 0;
  kelembapanSensor = 0;
  suhuSensor = 0;
  sessionId = "";
  tempProtectionActive = false;
  humidityControlActive = false;
  temperatureControlActive = false;

  manualHeaterControl = false;
  manualFanColControl = false;
  manualFanPanControl = false;
  manualExhaustControl = false;
  
  ErrorManager::clearAllErrors();
  
  currentInfoMode = INFO_DEFAULT;
  infoDisplayActive = false;
  
  tareButton = {false, false, 0, 0, 0, false};
  startButton = {false, false, 0, 0, 0, false};
  infoButton = {false, false, 0, 0, 0, false};
  
  Serial.println("✅ System restart completed");
  
  FirebaseManager::sendSystemStatus("restarted system");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RESTART SUCCESS");
  lcd.setCursor(0, 1);
  lcd.print("System Ready!");
  delay(2000);
  
  DisplayManager::updateDefault();
  
  Serial.println("🎉 LONG PRESS restart sequence completed!");
}

void ButtonHandler::testPins() {
  Serial.println("\n🔧 TESTING BUTTON PINS...");
  
  Serial.print("   DOOR (Pin 23): ");
  Serial.println(digitalRead(DOOR_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   TARE (Pin 32): ");
  Serial.println(digitalRead(TARE_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   START (Pin 33): ");
  Serial.println(digitalRead(START_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.print("   INFO (Pin 35): ");
  Serial.println(digitalRead(INFO_SWITCH) ? "HIGH (OK)" : "LOW");
  
  Serial.println("✅ Button pin test completed");
}