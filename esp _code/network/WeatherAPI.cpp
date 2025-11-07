#include "WeatherAPI.h"

void WeatherAPI::fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=Yogyakarta,id&appid=YOUR_API_KEY&units=metric";
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        String apiWeatherMain = doc["weather"][0]["main"].as();
        weatherDesc = doc["weather"][0]["description"].as();
        tempOutside = doc["main"]["temp"].as();

        // Konversi ke format Indonesia
        if (apiWeatherMain == "Clear") {
          weatherMain = "cerah";
        } else if (apiWeatherMain == "Clouds") {
          weatherMain = "berawan";
        } else if (apiWeatherMain == "Rain" || apiWeatherMain == "Drizzle" || apiWeatherMain == "Thunderstorm") {
          weatherMain = "hujan";
        } else {
          weatherMain = "berawan";
        }

        Serial.println("☀️ Cuaca: " + weatherMain + " (" + weatherDesc + ")");
        Serial.println("🌡️ Suhu luar: " + String(tempOutside) + "°C");

        Firebase.setString(firebaseData, "/weather/main", weatherMain);
        Firebase.setString(firebaseData, "/weather/desc", weatherDesc);
        Firebase.setFloat(firebaseData, "/weather/temp", tempOutside);
      }
    } else {
      Serial.println("❌ Gagal ambil data cuaca: " + String(httpCode));
    }
    http.end();
  }
}