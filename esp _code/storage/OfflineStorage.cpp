#include "OfflineStorage.h"
#include "../utils/TimeUtils.h"

void OfflineStorage::save() {
  DataPoint dp;
  dp.timestamp = TimeUtils::getTimestamp();
  dp.temperature = suhuSensor;
  dp.humidity = kelembapanSensor;
  dp.moisture = kadarAir;
  dp.weight = beratSekarang;
  dp.status = processStatus;
  dp.heaterStatus = pemanasStatus;
  dp.fanStatus = kipasStatus;
  dp.exhaustStatus = exhaustStatus;
  
  if (WiFi.status() != WL_CONNECTED) {
    offlineBuffer.push_back(dp);
    offlineMode = true;
    
    if (offlineBuffer.size() > 100) {
      offlineBuffer.erase(offlineBuffer.begin());
    }
    
    Serial.println("Data saved to offline buffer");
  }
}

void OfflineStorage::sync() {
  if (offlineBuffer.empty()) return;
  
  Serial.println("🔄 Syncing offline data to Firebase...");
  
  for (auto& dp : offlineBuffer) {
    String path = "/sessions/" + sessionId + "/data/" + String(millis());
    
    FirebaseJson json;
    json.set("timestamp", dp.timestamp);
    json.set("temperature", dp.temperature);
    json.set("humidity", dp.humidity);
    json.set("moisture", dp.moisture);
    json.set("weight", dp.weight);
    json.set("status", dp.status);
    json.set("heater", dp.heaterStatus);
    json.set("fan", dp.fanStatus);
    json.set("exhaust", dp.exhaustStatus);
    
    if (Firebase.setJSON(firebaseData, path, json)) {
      Serial.println("✅ Offline data synced: " + dp.timestamp);
    } else {
      Serial.println("❌ Failed to sync: " + dp.timestamp);
      break;
    }
    
    delay(100);
  }
  
  offlineBuffer.clear();
  offlineMode = false;
  Serial.println("All offline data synced successfully!");
}