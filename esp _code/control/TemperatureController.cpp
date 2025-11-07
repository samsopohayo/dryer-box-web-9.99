#include "TemperatureController.h"
#include "../system/ErrorManager.h"

void TemperatureController::checkProtection() {
  if (suhuSensor > MAX_TEMP_LIMIT) {
    if (!tempProtectionActive) {
      tempProtectionActive = true;
      humidityControlActive = false;
      temperatureControlActive = false;
      
      ErrorManager::addError("TEMP_PROTECTION", "Temperature too high: " + String(suhuSensor, 1) + "°C");
      Serial.println("🚨 TEMPERATURE PROTECTION ACTIVATED!");
      
      // Emergency cooling
      digitalWrite(RELAY1_PIN, RELAY_OFF);
      digitalWrite(RELAY2_PIN, RELAY_OFF);
      digitalWrite(MOSFET1_PIN, HIGH);
      digitalWrite(MOSFET2_PIN, HIGH);
      ledcWrite(FANHEATPAN1, 255);
      ledcWrite(FANHEATCOL2, 255);
      exhaustServo.write(90);
      
      pemanasStatus = "PROTECTION";
      kipasStatus = "COOLING";
      exhaustStatus = "COOLING";
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("OVER HEAT !!!");
      lcd.setCursor(0, 1);
      lcd.print("T:" + String(suhuSensor, 0) + "C>>" + String(MAX_TEMP_LIMIT, 0) + "C");
    }
  } else if (tempProtectionActive && suhuSensor < (MAX_TEMP_LIMIT - 5)) {
    tempProtectionActive = false;
    ErrorManager::clearError("TEMP_PROTECTION");
    Serial.println("✅ Temperature protection deactivated");
  }
}

void TemperatureController::checkControl() {
  if (!systemRunning || tempProtectionActive || doorOpen) {
    temperatureControlActive = false;
    return;
  }
  
  if (isnan(suhuSensor)) {
    temperatureControlActive = false;
    return;
  }
  
  if (suhuSensor > (targetTemperatureMax + TEMPERATURE_HYSTERESIS)) {
    if (!temperatureControlActive) {
      temperatureControlActive = true;
      Serial.println("🌡  TEMPERATURE CONTROL ACTIVATED!");
      Serial.println("   Current: " + String(suhuSensor, 1) + "°C > Target: " + String(targetTemperatureMax, 1) + "°C");
    }
    
    controlExhaust(suhuSensor);
    
  } else if (suhuSensor < (targetTemperatureMin - TEMPERATURE_HYSTERESIS)) {
    if (temperatureControlActive) {
      temperatureControlActive = false;
      Serial.println("🌡  TEMPERATURE CONTROL DEACTIVATED!");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
    
  } else if (suhuSensor >= targetTemperatureMin && suhuSensor <= targetTemperatureMax) {
    if (temperatureControlActive) {
      temperatureControlActive = false;
      Serial.println("🌡  TEMPERATURE IN IDEAL RANGE!");
      
      digitalWrite(MOSFET1_PIN, LOW);
      exhaustServo.write(0);
      exhaustStatus = "Tutup";
    }
  }
}

void TemperatureController::controlExhaust(float temperature) {
  float excess = temperature - targetTemperatureMax;
  
  if (excess <= 2.0) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(45);
    exhaustStatus = "Sedang-Temp";
    Serial.println("🌬  Exhaust SEDANG - Excess temp: " + String(excess, 1) + "°C");
    
  } else if (excess <= 5.0) {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(70);
    exhaustStatus = "Cepat-Temp";
    Serial.println("🌬  Exhaust CEPAT - Excess temp: " + String(excess, 1) + "°C");
    
  } else {
    digitalWrite(MOSFET1_PIN, HIGH);
    exhaustServo.write(90);
    exhaustStatus = "Maksimal-Temp";
    Serial.println("🌬  Exhaust MAKSIMAL - Excess temp: " + String(excess, 1) + "°C");
  }
}