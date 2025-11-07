#include "SensorManager.h"
#include "../system/ErrorManager.h"
#include "../network/FirebaseManager.h"

void SensorManager::init() {
  // Initialize DHT22
  dht.begin();
  delay(2000);
  
  // Initialize HX711
  scale.begin(DOUT_PIN, SCK_PIN);
  scale.set_scale(239.12);
  
  Serial.println("✅ Sensors initialized");
}

void SensorManager::readAll() {
  Serial.println("\n📊 === READING SENSORS ===");
  readDHT();
  readLoadCell();
}

void SensorManager::readDHT() {
  suhuSensor = dht.readTemperature();
  kelembapanSensor = dht.readHumidity();
  
  // Temperature validation
  if (isnan(suhuSensor)) {
    ErrorManager::addError("DHT22_TEMP_FAILED", "Temperature sensor error");
    suhuSensor = 0;
  } else {
    ErrorManager::clearError("DHT22_TEMP_FAILED");
    Serial.println("✅ DHT22 temperature: " + String(suhuSensor, 1) + "°C");
    
    if (suhuSensor > targetTemperatureMax) {
      Serial.println("   ⚠  Temperature HIGH - will activate cooling");
    } else if (suhuSensor < targetTemperatureMin) {
      Serial.println("   ⚠  Temperature LOW - normal operation");
    } else {
      Serial.println("   ✅ Temperature IDEAL - in target range");
    }
  }
  
  // Humidity validation
  if (isnan(kelembapanSensor)) {
    ErrorManager::addError("DHT22_HUMID_FAILED", "Humidity sensor error");
    kelembapanSensor = 0;
  } else {
    ErrorManager::clearError("DHT22_HUMID_FAILED");
    Serial.println("✅ DHT22 humidity: " + String(kelembapanSensor, 1) + "%");
    
    if (kelembapanSensor > targetHumidityMax) {
      Serial.println("   ⚠  Humidity HIGH - Exhaust will activate");
    } else if (kelembapanSensor < targetHumidityMin) {
      Serial.println("   ⚠  Humidity LOW - Exhaust will stop");
    } else {
      Serial.println("   ✅ Humidity IDEAL - in target range");
    }
  }
}

void SensorManager::readLoadCell() {
  if (!scale.is_ready()) {
    ErrorManager::addError("SCALE_NOT_READY", "Load cell not ready");
    beratSekarang = beratAwal;
  } else {
    ErrorManager::clearError("SCALE_NOT_READY");
    beratSekarang = scale.get_units(5);
    Serial.println("✅ HX711 weight: " + String(beratSekarang, 1) + " gram");
  }
}

void SensorManager::checkObjectWeight() {
  if (!scale.is_ready()) {
    ErrorManager::addError("SCALE_NOT_READY", "Scale not ready");
    return;
  }

  ErrorManager::clearError("SCALE_NOT_READY");
  float weight = scale.get_units(5);

  if (weight > 5.0) {
    if (abs(weight - lastWeight) <= 7.0) {
      if (!stabilizing) {
        stabilizing = true;
        stableStartTime = millis();
      } else {
        if (millis() - stableStartTime >= 4000) {
          beratAwal = weight;
          Serial.println("📦 Object detected");
          Serial.println("📦 Initial weight: " + String(beratAwal, 1) + " gram");

          currentState = WAIT_START;
          processStatus = "Siap";

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OBJEK TERDETEKSI");
          lcd.setCursor(0, 1);
          lcd.print("Berat:" + String(beratAwal, 0) + "g");
          delay(2000);

          Firebase.setFloat(firebaseData, "/sensor/berat_awal", beratAwal);

          stabilizing = false;
        }
      }
    } else {
      stabilizing = false;
    }
  } else {
    stabilizing = false;
  }
  lastWeight = weight;
}

void SensorManager::testDHT() {
  float testTemp = dht.readTemperature();
  float testHumid = dht.readHumidity();
  
  if (isnan(testTemp)) {
    ErrorManager::addError("DHT22_TEMP_FAILED", "Temperature sensor check wiring");
    Serial.println("❌ DHT22 temperature sensor ERROR");
  } else {
    ErrorManager::clearError("DHT22_TEMP_FAILED");
    Serial.println("✅ DHT22 temperature sensor OK - Temp: " + String(testTemp) + "°C");
  }
  
  if (isnan(testHumid)) {
    ErrorManager::addError("DHT22_HUMID_FAILED", "Humidity sensor check wiring");
    Serial.println("❌ DHT22 humidity sensor ERROR");
  } else {
    ErrorManager::clearError("DHT22_HUMID_FAILED");
    Serial.println("✅ DHT22 humidity sensor OK - RH: " + String(testHumid) + "%");
  }
}

void SensorManager::testLoadCell() {
  if (scale.is_ready()) {
    scale.set_scale(239.12);
    Serial.println("✅ HX711 load cell initialized");
    ErrorManager::clearError("SCALE_NOT_READY");
  } else {
    ErrorManager::addError("SCALE_NOT_READY", "Load cell check wiring");
    Serial.println("❌ HX711 load cell ERROR");
  }
}