#include "FirebaseManager.h"
#include "../system/ErrorManager.h"
#include "../utils/TimeUtils.h"

void FirebaseManager::setup() {
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  
  if (WiFi.status() == WL_CONNECTED) {
    if (Firebase.setString(firebaseData, "/system/status", "initializing")) {
      Serial.println("✅ Firebase connected successfully!");
      ErrorManager::clearError("FIREBASE_FAILED");
      sendSystemInfo();
    } else {
      ErrorManager::addError("FIREBASE_FAILED", "Firebase connection failed");
      Serial.println("❌ Firebase connection ERROR!");
    }
  }
}

void FirebaseManager::loadConfig() {
  if (Firebase.getFloat(firebaseData, "/config/targetMoisture")) {
    targetMoisture = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMin")) {
    targetHumidityMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMax")) {
    targetHumidityMax = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetTemperatureMin")) {
    targetTemperatureMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetTemperatureMax")) {
    targetTemperatureMax = firebaseData.floatData();
  }

  Serial.println("📥 Config loaded from Firebase:");
  Serial.println("  Target Moisture: " + String(targetMoisture));
  Serial.println("  Humidity Range: " + String(targetHumidityMin) + "-" + String(targetHumidityMax));
  Serial.println("  Temperature Range: " + String(targetTemperatureMin) + "-" + String(targetTemperatureMax));
}

void FirebaseManager::listenConfigUpdates() {
  if (Firebase.getFloat(firebaseData, "/config/targetMoisture")) {
    targetMoisture = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMin")) {
    targetHumidityMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetHumidityMax")) {
    targetHumidityMax = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetTemperatureMin")) {
    targetTemperatureMin = firebaseData.floatData();
  }
  if (Firebase.getFloat(firebaseData, "/config/targetTemperatureMax")) {
    targetTemperatureMax = firebaseData.floatData();
  }
}

void FirebaseManager::sendSystemStatus(String status) {
  if (WiFi.status() == WL_CONNECTED) {
    Firebase.setString(firebaseData, "/system/status", status);
    Firebase.setString(firebaseData, "/system/last_boot", TimeUtils::getTimestamp());
    Firebase.setString(firebaseData, "/system/version", "v3.3_modular");
  }
}

void FirebaseManager::sendSystemInfo() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  Firebase.setString(firebaseData, "/system/wifi_ssid", WIFI_SSID);
  Firebase.setInt(firebaseData, "/system/wifi_rssi", WiFi.RSSI());
  Firebase.setString(firebaseData, "/system/ip_address", WiFi.localIP().toString());
  
  bool dht22_ok = !isnan(dht.readTemperature()) && !isnan(dht.readHumidity());
  bool hx711_ok = scale.is_ready();
  
  Firebase.setBool(firebaseData, "/system/sensor_dht22", dht22_ok);
  Firebase.setBool(firebaseData, "/system/sensor_hx711", hx711_ok);
  
  Serial.println("📤 System info sent to Firebase");
}

void FirebaseManager::sendSessionStart() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  String sessionPath = "/sessions/" + sessionId;
  
  FirebaseJson json;
  json.set("session_id", sessionId);
  json.set("start_time", TimeUtils::getTimestamp());
  json.set("initial_weight", beratAwal);
  json.set("initial_humidity", kelembapanSensor);
  json.set("humidity_target_min", targetHumidityMin);
  json.set("humidity_target_max", targetHumidityMax);
  json.set("temperature_target_min", targetTemperatureMin);
  json.set("temperature_target_max", targetTemperatureMax);
  json.set("initial_temperature", suhuSensor);
  json.set("status", "active");
  
  Firebase.setJSON(firebaseData, sessionPath + "/info", json);
  Firebase.setString(firebaseData, "/system/current_session", sessionId);
  Firebase.setString(firebaseData, "/system/process_status", "running");
}

void FirebaseManager::sendSessionEnd() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  String sessionPath = "/sessions/" + sessionId;
  
  FirebaseJson json;
  json.set("end_time", TimeUtils::getTimestamp());
  json.set("final_weight", beratSekarang);
  json.set("final_moisture", kadarAir);
  json.set("final_humidity", kelembapanSensor);
  json.set("status", "completed");
  
  Firebase.updateNode(firebaseData, sessionPath + "/info", json);
  Firebase.setString(firebaseData, "/system/process_status", "completed");
}

void FirebaseManager::sendData() {
  static unsigned long lastSend = 0;
  if (millis() - lastSend < 1000) {
    Serial.println("⚠️ Firebase send throttled");
    return;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected");
    offlineMode = true;
    return;
  }
  
  lastSend = millis();
  Serial.println("\n📤 === SENDING DATA TO FIREBASE ===");
  
  sendSensorData();
  sendStatusData();
  
  if (systemRunning && !sessionId.isEmpty()) {
    sendSessionData();
  }
  
  Firebase.setString(firebaseData, "/system/last_update", TimeUtils::getTimestamp());
  
  static unsigned long lastSystemInfoSend = 0;
  if (millis() - lastSystemInfoSend > 30000) {
    sendSystemInfo();
    lastSystemInfoSend = millis();
  }
}

void FirebaseManager::sendSensorData() {
  Firebase.setFloat(firebaseData, "/sensor/suhu", suhuSensor);
  delay(50);
  Firebase.setFloat(firebaseData, "/sensor/humidity", kelembapanSensor);
  delay(50);
  Firebase.setFloat(firebaseData, "/sensor/kelembaban", kadarAir);
  delay(50);
  Firebase.setFloat(firebaseData, "/sensor/berat", beratSekarang);
  delay(50);
}

void FirebaseManager::sendStatusData() {
  Firebase.setString(firebaseData, "/status/pengeringan", processStatus);
  delay(50);
  Firebase.setString(firebaseData, "/status/pemanas", pemanasStatus);
  delay(50);
  Firebase.setString(firebaseData, "/status/kipas", kipasStatus);
  delay(50);
  Firebase.setString(firebaseData, "/status/exhaust", exhaustStatus);
  delay(50);
  Firebase.setBool(firebaseData, "/status/door_open", doorOpen);
  delay(50);
  Firebase.setBool(firebaseData, "/status/temp_protection", tempProtectionActive);
  delay(50);
  Firebase.setBool(firebaseData, "/status/humidity_control", humidityControlActive);
  delay(50);
  Firebase.setBool(firebaseData, "/status/temperature_control", temperatureControlActive);
  delay(50);
}

void FirebaseManager::sendSessionData() {
  String sessionPath = "/sessions/" + sessionId + "/data/" + String(millis());
  
  FirebaseJson json;
  json.set("timestamp", TimeUtils::getTimestamp());
  json.set("temperature", suhuSensor);
  json.set("humidity", kelembapanSensor);
  json.set("moisture", kadarAir);
  json.set("weight", beratSekarang);
  json.set("status", processStatus);
  json.set("heater", pemanasStatus);
  json.set("fan", kipasStatus);
  json.set("exhaust", exhaustStatus);
  json.set("humidity_control", humidityControlActive);
  json.set("temperature_control", temperatureControlActive);

  Firebase.setJSON(firebaseData, sessionPath, json);
  delay(50);
}