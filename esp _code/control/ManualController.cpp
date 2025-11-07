#include "ManualController.h"

void ManualController::check() {
  static unsigned long lastManualCheck = 0;
  if (millis() - lastManualCheck < 5000) {
    return;
  }
  lastManualCheck = millis();

  if (WiFi.status() != WL_CONNECTED) return;
  
  Serial.println("🔍 Checking manual controls from Firebase...");
  
  if (Firebase.getBool(firebaseData, "/control/manual_heater_enable")) {
    manualHeaterControl = firebaseData.boolData();
  }
  delay(50);
  
  if (Firebase.getBool(firebaseData, "/control/manual_heater_state")) {
    manualHeaterState = firebaseData.boolData();
  }
  delay(50);
  
  if (Firebase.getBool(firebaseData, "/control/manual_fan_collector_enable")) {
    manualFanColControl = firebaseData.boolData();
  }
  delay(50);
  
  if (Firebase.getBool(firebaseData, "/control/manual_fan_collector_state")) {
    manualFanColState = firebaseData.boolData();
  }
  delay(50);

  if (Firebase.getBool(firebaseData, "/control/manual_fan_panel_enable")) {
    manualFanPanControl = firebaseData.boolData();
  }
  delay(50);
  
  if (Firebase.getBool(firebaseData, "/control/manual_fan_panel_state")) {
    manualFanPanState = firebaseData.boolData();
  }
  delay(50);

  if (Firebase.getBool(firebaseData, "/control/manual_exhaust_enable")) {
    manualExhaustControl = firebaseData.boolData();
  }
  delay(50);
  
  if (Firebase.getBool(firebaseData, "/control/manual_exhaust_state")) {
    manualExhaustState = firebaseData.boolData();
  }
  delay(50);
  
  Serial.println("✅ Manual controls synced");
}

void ManualController::apply() {
  Serial.println("\n⚙  === MANUAL CONTROL MODE ===");
  
  if (manualHeaterControl) {
    controlHeater(manualHeaterState);
    Serial.println("   Manual heater override - fuzzy control disabled");
  }
  
  if (manualFanColControl) {
    controlFanCollector(manualFanColState);
    Serial.println("   Manual fan collector override - fuzzy control disabled");
  }
  
  if (manualFanPanControl) {
    controlFanPanel(manualFanPanState);
    Serial.println("   Manual fan panel override - fuzzy control disabled");
  }

  if (manualExhaustControl) {
    controlExhaust(manualExhaustState);
    Serial.println("   Manual exhaust override - humidity control disabled");
  }
}

void ManualController::controlHeater(bool state) {
  if (state) {
    digitalWrite(RELAY1_PIN, RELAY_ON);
    digitalWrite(RELAY2_PIN, RELAY_ON);
    pemanasStatus = "Manual ON";
  } else {
    digitalWrite(RELAY1_PIN, RELAY_OFF);
    digitalWrite(RELAY2_PIN, RELAY_OFF);
    pemanasStatus = "Manual OFF";
  }
  Serial.println("🔥 Heater Manual: " + pemanasStatus);
}

void ManualController::controlFanPanel(bool state) {
  if (state) {
    ledcWrite(FANHEATPAN1, 255);
    kipasStatus = "Pan Manual ON";
  } else {
    ledcWrite(FANHEATPAN1, 0);
    kipasStatus = "Pan Manual OFF";
  }
  Serial.println("💨 Fan Panel Manual: " + kipasStatus);
}

void ManualController::controlFanCollector(bool state) {
  if (state) {
    ledcWrite(FANHEATCOL2, 255);
    kipasStatus = "Col Manual ON";
  } else {
    ledcWrite(FANHEATCOL2, 0);
    kipasStatus = "Col Manual OFF";
  }
  Serial.println("💨 Fan Collector Manual: " + kipasStatus);
}

void ManualController::controlExhaust(bool state) {
  if (state) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Manual ON";
  } else {
    digitalWrite(MOSFET1_PIN, LOW);
    exhaustServo.write(0);
    exhaustStatus = "Manual OFF";
  }
  Serial.println("🌬  Exhaust Manual: " + exhaustStatus);
}