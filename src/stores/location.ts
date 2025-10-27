// FILE: src/stores/location.ts (NEW - Weather & Location Store)
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";
import { database } from "@/firebase/config";
import { ref as dbRef, set, onValue } from "firebase/database";
import axios from "axios";

interface LocationData {
  city: string;
  lat: number;
  lon: number;
}

interface WeatherData {
  condition: "cerah" | "berawan" | "hujan" | "-";
  description: string;
  temperature: number;
  humidity: number;
}

export const useLocationStore = defineStore("location", () => {
  const location = ref<LocationData>({
    city: "-",
    lat: 0,
    lon: 0,
  });

  const weather = ref<WeatherData>({
    condition: "-",
    description: "-",
    temperature: 0,
    humidity: 5,
  });

  const isLoading = ref(false);
  const error = ref<string | null>(null);

  // Initialize listener untuk location dari Firebase
  const initializeListener = () => {
    onValue(dbRef(database, "location"), (snapshot) => {
      if (snapshot.exists()) {
        location.value = snapshot.val();
        fetchWeather();
      }
    });
  };

  // Geocoding: Convert city name to coordinates
  const geocodeCity = async (
    cityName: string
  ): Promise<{ lat: number; lon: number } | null> => {
    try {
      const response = await axios.get(
        `https://geocoding-api.open-meteo.com/v1/search?name=${encodeURIComponent(
          cityName
        )}&count=1&language=id&format=json`
      );

      if (response.data.results && response.data.results.length > 0) {
        const result = response.data.results[0];
        return {
          lat: result.latitude,
          lon: result.longitude,
        };
      }
      return null;
    } catch (err) {
      console.error("Geocoding error:", err);
      return null;
    }
  };

  // Fetch weather dari Open-Meteo API
  const fetchWeather = async () => {
    isLoading.value = true;
    error.value = null;

    try {
      const response = await axios.get(
        `https://api.open-meteo.com/v1/forecast?latitude=${location.value.lat}&longitude=${location.value.lon}&current=temperature_2m,relative_humidity_2m,weather_code&timezone=Asia/Jakarta`
      );

      const currentData = response.data.current;
      const weatherCode = currentData.weather_code;

      // Konversi WMO Weather Code ke kondisi sederhana
      let condition: "cerah" | "berawan" | "hujan";
      let description: string;

      if (weatherCode === 0) {
        condition = "cerah";
        description = "Cerah";
      } else if ([1, 2, 3].includes(weatherCode)) {
        condition = "berawan";
        description = "Berawan";
      } else if ([45, 48].includes(weatherCode)) {
        condition = "berawan";
        description = "Berkabut";
      } else if ([51, 53, 55, 56, 57].includes(weatherCode)) {
        condition = "hujan";
        description = "Gerimis";
      } else if ([61, 63, 65, 66, 67, 80, 81, 82].includes(weatherCode)) {
        condition = "hujan";
        description = "Hujan";
      } else if ([71, 73, 75, 77, 85, 86].includes(weatherCode)) {
        condition = "hujan";
        description = "Salju/Hujan Lebat";
      } else if ([95, 96, 99].includes(weatherCode)) {
        condition = "hujan";
        description = "Badai Petir";
      } else {
        condition = "berawan";
        description = "Berawan";
      }

      weather.value = {
        condition,
        temperature: Math.round(currentData.temperature_2m),
        humidity: currentData.relative_humidity_2m,
        description,
      };

      // Kirim ke Firebase untuk ESP32
      await set(dbRef(database, "weather"), {
        main: condition,
        desc: description,
        temp: currentData.temperature_2m,
      });
    } catch (err) {
      console.error("Weather fetch error:", err);
      error.value = "Gagal mengambil data cuaca";
    } finally {
      isLoading.value = false;
    }
  };

  // Update lokasi
  const updateLocation = async (cityName: string) => {
    isLoading.value = true;
    error.value = null;

    try {
      const coords = await geocodeCity(cityName);

      if (!coords) {
        error.value = "Kota tidak ditemukan";
        return false;
      }

      const newLocation = {
        city: cityName,
        lat: coords.lat,
        lon: coords.lon,
      };

      // Simpan ke Firebase
      await set(dbRef(database, "location"), newLocation);

      location.value = newLocation;

      // Fetch weather baru
      await fetchWeather();

      return true;
    } catch (err) {
      console.error("Update location error:", err);
      error.value = "Gagal mengupdate lokasi";
      return false;
    } finally {
      isLoading.value = false;
    }
  };

  return {
    location,
    weather,
    isLoading,
    error,
    initializeListener,
    updateLocation,
    fetchWeather,
  };
});
