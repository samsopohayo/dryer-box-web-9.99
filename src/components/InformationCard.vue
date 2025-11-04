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
          <p class="font-semibold text-gray-800 dark:text-white truncate">
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
            :class="rssiSignalClass"
            class="font-semibold px-2 py-1 rounded-lg inline-block text-xs sm:text-sm"
          >
            {{ wifiRSSI }} dBm — {{ rssiSignalText }}
          </p>
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
          <p
            class="font-semibold truncate"
            :class="
              sensorDHT22 === 'OK'
                ? 'text-green-600 dark:text-green-400'
                : 'text-red-600 dark:text-red-400'
            "
          >
            {{ sensorDHT22 }}
          </p>
        </div>
      </div>

      <!-- Sensor HX711 -->
      <div class="flex items-start gap-3">
        <Gauge class="w-5 h-5 text-green-600 mt-0.5 flex-shrink-0" />
        <div class="min-w-0">
          <p class="text-gray-600 dark:text-gray-400">Status Sensor HX711:</p>
          <p
            class="font-semibold truncate"
            :class="
              sensorHX711 === 'OK'
                ? 'text-green-600 dark:text-green-400'
                : 'text-red-600 dark:text-red-400'
            "
          >
            {{ sensorHX711 }}
          </p>
        </div>
      </div>

      <!-- Error Status -->
      <div
        class="flex items-start gap-3 col-span-1 sm:col-span-2 lg:col-span-4"
      >
        <AlertCircle class="w-5 h-5 text-red-500 mt-0.5 flex-shrink-0" />
        <div class="min-w-0 flex-1">
          <p class="text-gray-600 dark:text-gray-400">Status Error:</p>
          <p
            class="font-semibold break-words"
            :class="
              errorStatus === 'None' || errorStatus === 'No active errors'
                ? 'text-green-600 dark:text-green-400'
                : 'text-red-600 dark:text-red-400'
            "
          >
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
} from "lucide-vue-next";

const dryerStore = useDryerStore();

const networkStatus = computed(() => {
  return dryerStore.systemStatus.status === "ready" ? "Stabil" : "Tidak Stabil";
});

const wifiRSSI = computed(() => dryerStore.systemStatus.wifi_rssi || "-");

const rssiSignalClass = computed(() => {
  const rssi = wifiRSSI.value;
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
  if (rssi >= -50) return "Excellent";
  if (rssi >= -60) return "Good";
  if (rssi >= -70) return "Fair";
  return "Weak";
});

const ipAddress = computed(() => dryerStore.systemStatus.ip_address || "-");

const sensorDHT22 = computed(() =>
  dryerStore.sensorData.suhu > 0 ? "OK" : "ERROR"
);

const sensorHX711 = computed(() =>
  dryerStore.sensorData.berat >= 0 ? "OK" : "ERROR"
);

const errorStatus = computed(
  () => dryerStore.systemStatus.last_error || "None"
);
</script>
