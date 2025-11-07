#include "WiFiManager.h"
#include "../system/ErrorManager.h"

void WiFiManager::connect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi..");
  lcd.setCursor(0, 1);
  lcd.print("SSID: " + String(WIFI_SSID));
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("🌐 Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    lcd.setCursor(14 + (attempts % 3), 0);
    lcd.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✅ WiFi connected successfully!");
    Serial.println("   IP: " + WiFi.localIP().toString());
    Serial.println("   RSSI: " + String(WiFi.RSSI()) + " dBm");
    ErrorManager::clearError("WIFI_FAILED");
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    lcd.print("IP:" + WiFi.localIP().toString().substring(0, 12));
    delay(2000);
    
  } else {
    Serial.println();
    ErrorManager::addError("WIFI_FAILED", "WiFi connection failed");
    offlineMode = true;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Offline Mode");
    delay(2000);
  }
}

bool WiFiManager::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

int WiFiManager::getRSSI() {
  return WiFi.RSSI();
}