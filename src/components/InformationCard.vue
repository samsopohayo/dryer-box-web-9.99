<template>
  <div class="h-[30px]"></div>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
  >
    <h2
      class="text-base sm:text-lg font-semibold mb-4 text-gray-800 dark:text-white flex items-center gap-2"
    >
      <Cpu class="w-5 h-5 text-blue-500" /> Informasi Sistem
    </h2>

    <div
      class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6 text-sm"
    >
      <!-- Status jaringan -->
      <div class="flex items-start gap-3">
        <Network class="w-5 h-5 text-blue-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Koneksi Jaringan:</p>
          <p
            class="font-semibold truncate"
            :class="
              isConnected
                ? 'text-green-600 dark:text-green-400'
                : 'text-red-600 dark:text-red-400'
            "
          >
            {{ networkStatus }}
          </p>
        </div>
      </div>

      <!-- Nama SSID -->
      <div class="flex items-start gap-3">
        <Wifi class="w-5 h-5 text-green-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Nama SSID:</p>
          <p class="font-semibold text-gray-800 dark:text-white truncate">
            {{ wifiSSID }}
          </p>
        </div>
      </div>

      <!-- Kekuatan sinyal WiFi -->
      <div class="flex items-start gap-3">
        <Signal class="w-5 h-5 text-yellow-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Kekuatan Sinyal WiFi:</p>
          <p
            v-if="isConnected"
            :class="rssiSignalClass"
            class="font-semibold px-2 py-1 rounded-lg inline-block text-xs sm:text-sm"
          >
            {{ wifiRSSI }} dBm — {{ rssiSignalText }}
          </p>
          <p v-else class="font-semibold text-gray-400">-</p>
        </div>
      </div>

      <!-- IP Address -->
      <div class="flex items-start gap-3">
        <Network class="w-5 h-5 text-indigo-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Alamat IP:</p>
          <p class="font-semibold text-gray-800 dark:text-white truncate">
            {{ ipAddress }}
          </p>
        </div>
      </div>

      <!-- Sensor DHT22 -->
      <div class="flex items-start gap-3">
        <Thermometer class="w-5 h-5 text-red-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Status Sensor DHT22:</p>
          <p class="font-semibold truncate" :class="sensorDHT22Class">
            {{ sensorDHT22 }}
          </p>
        </div>
      </div>

      <!-- Sensor HX711 -->
      <div class="flex items-start gap-3">
        <Gauge class="w-5 h-5 text-green-600 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Status Sensor HX711:</p>
          <p class="font-semibold truncate" :class="sensorHX711Class">
            {{ sensorHX711 }}
          </p>
        </div>
      </div>

      <!-- Last Update -->
      <div class="flex items-start gap-3">
        <Clock class="w-5 h-5 text-purple-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Update Terakhir:</p>
          <p class="font-semibold text-gray-800 dark:text-white truncate">
            {{ lastUpdate }}
          </p>
        </div>
      </div>

      <!-- Error Status -->
      <div class="flex items-start gap-3">
        <AlertCircle class="w-5 h-5 text-red-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0 flex-1">
          <p class="text-gray-600 dark:text-gray-400">Status Error:</p>
          <p class="font-semibold break-words" :class="errorStatusClass">
            {{ errorStatus }}
          </p>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from "vue";
import { useDryerStore } from "@/stores/dryer";
import {
  Cpu,
  Network,
  Wifi,
  Signal,
  Thermometer,
  Gauge,
  AlertCircle,
  CheckCircle,
  Clock,
} from "lucide-vue-next";

const dryerStore = useDryerStore();

// Check if system is connected (last update within 1 minute)
const isConnected = computed(() => {
  const lastUpdate = dryerStore.systemStatus.last_update;
  if (!lastUpdate) return false;

  // Parse timestamp "DD/MM/YYYY HH:MM:SS"
  const parseTimestamp = (ts: string): Date => {
    const [date, time] = ts.split(" ");
    const [day, month, year] = date.split("/");
    const [hours, minutes, seconds] = time.split(":");
    return new Date(
      parseInt(year),
      parseInt(month) - 1,
      parseInt(day),
      parseInt(hours),
      parseInt(minutes),
      parseInt(seconds)
    );
  };

  const lastUpdateTime = parseTimestamp(lastUpdate);
  const now = new Date();
  const diffMs = now.getTime() - lastUpdateTime.getTime();
  const diffMinutes = diffMs / 60000;

  return diffMinutes < 1;
});

const networkStatus = computed(() => {
  return isConnected.value ? "Terhubung" : "Terputus";
});

const wifiSSID = computed(() => {
  return isConnected.value && dryerStore.systemStatus.wifi_ssid
    ? dryerStore.systemStatus.wifi_ssid
    : "-";
});

const wifiRSSI = computed(() => {
  return isConnected.value && dryerStore.systemStatus.wifi_rssi
    ? dryerStore.systemStatus.wifi_rssi
    : 0;
});

const rssiSignalClass = computed(() => {
  const rssi = wifiRSSI.value;
  if (!isConnected.value) return "text-gray-400";
  if (rssi >= -50)
    return "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200";
  if (rssi >= -60)
    return "bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200";
  if (rssi >= -70)
    return "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200";
  return "bg-red-100 text-red-800 dark:bg-red-900 dark:text-red-200";
});

const rssiSignalText = computed(() => {
  const rssi = wifiRSSI.value;
  if (!isConnected.value) return "-";
  if (rssi >= -50) return "Excellent";
  if (rssi >= -60) return "Good";
  if (rssi >= -70) return "Fair";
  return "Weak";
});

const ipAddress = computed(() => {
  return isConnected.value && dryerStore.systemStatus.ip_address
    ? dryerStore.systemStatus.ip_address
    : "-";
});

const sensorDHT22 = computed(() => {
  if (!isConnected.value) return "N/A";
  if (dryerStore.systemStatus.sensor_dht22 === true) return "OK";
  if (dryerStore.systemStatus.sensor_dht22 === false) return "ERROR";
  // Fallback: check sensor data
  return dryerStore.sensorData.suhu > 0 ? "OK" : "ERROR";
});

const sensorDHT22Class = computed(() => {
  if (!isConnected.value) return "text-gray-400 dark:text-gray-500";
  return sensorDHT22.value === "OK"
    ? "text-green-600 dark:text-green-400"
    : "text-red-600 dark:text-red-400";
});

const sensorHX711 = computed(() => {
  if (!isConnected.value) return "N/A";
  if (dryerStore.systemStatus.sensor_hx711 === true) return "OK";
  if (dryerStore.systemStatus.sensor_hx711 === false) return "ERROR";
  // Fallback: check sensor data
  return dryerStore.sensorData.berat >= 0 ? "OK" : "ERROR";
});

const sensorHX711Class = computed(() => {
  if (!isConnected.value) return "text-gray-400 dark:text-gray-500";
  return sensorHX711.value === "OK"
    ? "text-green-600 dark:text-green-400"
    : "text-red-600 dark:text-red-400";
});

const lastUpdate = computed(() => {
  return isConnected.value && dryerStore.systemStatus.last_update
    ? dryerStore.systemStatus.last_update
    : "-";
});

const errorStatus = computed(() => {
  if (!isConnected.value) return "Sistem tidak terkoneksi";
  const error = dryerStore.systemStatus.last_error;
  if (!error || error === "None" || error === "No active errors") {
    return "Tidak ada error";
  }
  return error;
});

const errorStatusClass = computed(() => {
  if (!isConnected.value)
    return "text-orange-600 dark:text-orange-400 font-semibold";
  const error = dryerStore.systemStatus.last_error;
  if (!error || error === "None" || error === "No active errors") {
    return "text-green-600 dark:text-green-400";
  }
  return "text-red-600 dark:text-red-400";
});
</script>
