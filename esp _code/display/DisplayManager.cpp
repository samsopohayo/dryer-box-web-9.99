#include "DisplayManager.h"
#include "../system/ErrorManager.h"

void DisplayManager::init() {
  lcd.init();
  lcd.backlight();
  displayBoot();
  Serial.println("✅ LCD 16x2 initialized");
}

void DisplayManager::displayBoot() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SISTEM PENGERING");
  lcd.setCursor(0, 1);
  lcd.print("ESP32 v3.3");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LongPress TARE t");
  lcd.setCursor(0, 1);
  lcd.print("restart system");
  delay(3000);
}

void DisplayManager::update() {
  if (hasActiveErrors) {
    displayErrors();
  } else if (infoDisplayActive) {
    displayInfo();
  } else {
    updateDefault();
  }
}

void DisplayManager::updateDefault() {
  lcd.clear();
  
  switch (currentState) {
    case WAIT_TARE:
      lcd.setCursor(0, 0);
      lcd.print("Tekan TARE untuk");
      lcd.setCursor(0, 1);
      lcd.print("reset timbangan");
      break;

    case WAIT_OBJECT: {
      lcd.setCursor(0, 0);
      lcd.print("Masukkan objek");
      lcd.setCursor(0, 1);

      float currentWeight = scale.get_units(10);
      if (currentWeight < 0 || currentWeight < 5.0) {
        lcd.print("Timbangan kosong ");
      } else {
        lcd.print("Berat:" + String(currentWeight, 0) + "g    ");
      }
      break;
    }

    case WAIT_START:
      lcd.setCursor(0, 0);
      lcd.print("Awal:" + String(beratAwal, 0) + "g      ");
      lcd.setCursor(0, 1);
      lcd.print("Tekan START");
      break;
      
    case RUNNING:
      lcd.setCursor(0, 0);
      lcd.print("T:" + String(suhuSensor, 0) + " H:" + String(kelembapanSensor, 0) + " KA:" + String(kadarAir, 0));
      
      lcd.setCursor(0, 1);
      String statusLine = "";
      
      if (tempProtectionActive) {
        statusLine = "TEMP PROTECTION";
      } else if (doorOpen) {
        statusLine = "DOOR OPEN-PAUSED";
      } else {
        statusLine = "STATUS: " + processStatus;
      }
      
      while (statusLine.length() < 16) statusLine += " ";
      if (statusLine.length() > 16) statusLine = statusLine.substring(0, 16);
      
      lcd.print(statusLine);
      break;
  }
}

void DisplayManager::displayInfo() {
  lcd.clear();

  switch (currentInfoMode) {
    case INFO_SENSORS: {
      lcd.setCursor(0, 0);
      lcd.print("== SENSORS ==");
      lcd.setCursor(0, 1);
      String sensorStatus = "DHT:";
      sensorStatus += (isnan(dht.readTemperature()) ? "ERR" : "OK");
      sensorStatus += " HX711:";
      sensorStatus += (scale.is_ready() ? "OK" : "ERR");
      lcd.print(sensorStatus);
      break;
    }

    case INFO_WEIGHTS: {
      lcd.setCursor(0, 0);
      lcd.print("=== WEIGHTS ===");
      lcd.setCursor(0, 1);
      lcd.print("A:" + String(beratAwal, 0) + "g S:" + String(beratSekarang, 0) + "g");
      break;
    }

    case INFO_WIFI: {
      lcd.setCursor(0, 0);
      lcd.print("=== NETWORK ===");
      lcd.setCursor(0, 1);
      if (WiFi.status() == WL_CONNECTED) {
        lcd.print("WiFi:" + String(WiFi.RSSI()) + "dBm OK");
      } else {
        lcd.print("WiFi: OFFLINE");
      }
      break;
    }

    case INFO_HUMADITY: {
      lcd.setCursor(0, 0);
      lcd.print("== HUMIDITY ==");
      lcd.setCursor(0, 1);
      String humidStatus = String(kelembapanSensor, 0) + "% ";
      if (humidityControlActive) {
        humidStatus += "CTRL-ON";
      } else if (kelembapanSensor >= targetHumidityMin && kelembapanSensor <= targetHumidityMax) {
        humidStatus += "IDEAL";
      } else {
        humidStatus += "OK";
      }
      lcd.print(humidStatus);
      break;
    }

    case INFO_TEMP: {
      lcd.setCursor(0, 0);
      lcd.print("== TEMPERATURE ==");
      lcd.setCursor(0, 1);
      String temprStatus = String(suhuSensor, 0) + "C ";
      if (temperatureControlActive) {
        temprStatus += "CTRL-ON";
      } else if (suhuSensor >= targetTemperatureMin && suhuSensor <= targetTemperatureMax) {
        temprStatus += "IDEAL";
      } else {
        temprStatus += "OK";
      }
      lcd.print(temprStatus);
      break;
    }

    case INFO_HISTORY: {
      lcd.setCursor(0, 0);
      lcd.print("== STATUS ==");
      lcd.setCursor(0, 1);
      String statusLine = "";
      statusLine += " Htr:" + pemanasStatus.substring(0, 1);
      statusLine += " Fan:" + kipasStatus.substring(0, 1);
      statusLine += " Exh:" + exhaustStatus.substring(0, 1);
      statusLine += " Hum:" + String(humidityControlActive ? "!" : "OK");
      statusLine += " Tem:" + String(temperatureControlActive ? "!" : "OK");
      lcd.print(statusLine);
      break;
    }

    default:
      break;
  }
}

void DisplayManager::displayErrors() {
  for (int i = 0; i < errorCount; i++) {
    if (systemErrors[i].active) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ERROR !!!");
      lcd.setCursor(0, 1);
      
      String errorMsg = systemErrors[i].code;
      if (errorMsg.length() > 16) {
        errorMsg = errorMsg.substring(0, 16);
      }
      while (errorMsg.length() < 16) errorMsg += " ";
      
      lcd.print(errorMsg);
      break;
    }
  }
}

void DisplayManager::cycleInfo() {
  if (!infoDisplayActive) {
    infoDisplayActive = true;
    currentInfoMode = INFO_SENSORS;
    infoDisplayStart = millis();
  } else {
    switch (currentInfoMode) {
      case INFO_SENSORS: currentInfoMode = INFO_WEIGHTS; break;
      case INFO_WEIGHTS: currentInfoMode = INFO_WIFI; break;
      case INFO_WIFI: currentInfoMode = INFO_HUMADITY; break;
      case INFO_HUMADITY: currentInfoMode = INFO_TEMP; break;
      case INFO_TEMP: currentInfoMode = INFO_HISTORY; break;
      case INFO_HISTORY: currentInfoMode = INFO_SENSORS; break;
      default: currentInfoMode = INFO_SENSORS; break;
    }
    infoDisplayStart = millis();
  }
  
  Serial.println("ℹ  Info display mode: " + String(currentInfoMode));
}

void DisplayManager::handleInfoTimeout() {
  if (infoDisplayActive && (millis() - infoDisplayStart > INFO_DISPLAY_TIMEOUT)) {
    infoDisplayActive = false;
    currentInfoMode = INFO_DEFAULT;
    Serial.println("ℹ  Info display timeout - returning to default");
  }
}

void DisplayManager::updateDoorStatus() {
  bool currentDoorState = (digitalRead(DOOR_SWITCH) == HIGH);
  
  if (currentDoorState != doorOpen) {
    doorOpen = currentDoorState;
    
    if (doorOpen && systemRunning) {
      Serial.println("🚪 Door opened - Process PAUSED for safety");
      processStatus = "Paused";
      humidityControlActive = false;
      temperatureControlActive = false;
      
      extern void turnOffAllOutputs();
      turnOffAllOutputs();
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!PINTU TERBUKA!");
      lcd.setCursor(0, 1);
      lcd.print("Proses di-pause");
      
    } else if (!doorOpen && systemRunning && processStatus == "Paused") {
      Serial.println("🚪 Door closed - Process RESUMED");
      processStatus = "Berjalan";
    }
  }
}