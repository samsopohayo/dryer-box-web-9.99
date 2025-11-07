#include "HumidityController.h"

void HumidityController::check() {
  if (!systemRunning || tempProtectionActive || doorOpen) {
    humidityControlActive = false;
    return;
  }
  
  if (isnan(kelembapanSensor)) {
    humidityControlActive = false;
    return;
  }
  
  if (kelembapanSensor > (targetHumidityMax + HUMIDITY_HYSTERESIS)) {
    if (!humidityControlActive) {
      humidityControlActive = true;
      Serial.println("💧 HUMIDITY CONTROL ACTIVATED!");
      Serial.println("   Current: " + String(kelembapanSensor, 1) + "% > Target: " + String(targetHumidityMax, 1) + "%");
    }
    
    controlExhaust(kelembapanSensor);
    
  } else if (kelembapanSensor < (targetHumidityMin - HUMIDITY_HYSTERESIS)) {
    if (humidityControlActive) {
      humidityControlActive = false;
      Serial.println("💧 HUMIDITY CONTROL DEACTIVATED!");
      Serial.println("   Current: " + String(kelembapanSensor, 1) + "% < Target: " + String(targetHumidityMin, 1) + "%");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
    
  } else if (kelembapanSensor >= targetHumidityMin && kelembapanSensor <= targetHumidityMax) {
    if (humidityControlActive) {
      humidityControlActive = false;
      Serial.println("💧 HUMIDITY IN IDEAL RANGE!");
      Serial.println("   Current: " + String(kelembapanSensor, 1) + "% (Target: " + String(targetHumidityMin, 1) + "-" + String(targetHumidityMax, 1) + "%)");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
  }
}

void HumidityController::controlExhaust(float humidity) {
  float excess = humidity - targetHumidityMax;
  
  if (excess <= 5.0) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(45);
    exhaustStatus = "Sedang-Humid";
    Serial.println("🌬  Exhaust SEDANG - Excess humidity: " + String(excess, 1) + "%");
    
  } else if (excess <= 10.0) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(70);
    exhaustStatus = "Cepat-Humid";
    Serial.println("🌬  Exhaust CEPAT - Excess humidity: " + String(excess, 1) + "%");
    
  } else {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Maksimal-Humid";
    Serial.println("🌬  Exhaust MAKSIMAL - Excess humidity: " + String(excess, 1) + "%");
  }
}