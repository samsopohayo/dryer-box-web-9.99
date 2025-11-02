<!-- Dashboard.vue - Updated with proper responsive layout -->
<script setup lang="ts">
import { computed, onMounted, ref } from "vue";
import { useDryerStore } from "@/stores/dryer";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import SensorCard from "@/components/SensorCard.vue";
import StatusCard from "@/components/StatusCard.vue";
import ChartMonitoring from "@/components/ChartMonitoring.vue";
import {
  Wifi,
  Signal,
  Network,
  Cpu,
  Thermometer,
  Gauge,
  AlertCircle,
} from "lucide-vue-next";

const dryerStore = useDryerStore();
const chartData = ref<any[]>([]);
const loading = ref(true);
const isSidebarCollapsed = ref(true);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const networkStatus = computed(() => {
  return dryerStore.systemStatus.status === "ready" ? "Stabil" : "Tidak Stabil";
});

const wifiSSID = computed(() => dryerStore.systemStatus.wifi_ssid || "-");
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

onMounted(async () => {
  loading.value = true;
  try {
    if (dryerStore.systemStatus.current_session) {
      await dryerStore.fetchSessionData(
        dryerStore.systemStatus.current_session
      );
      const dataPoints: any[] = Object.values(
        dryerStore.currentSessionData || {}
      );

      const timeSlots = Array.from({ length: 12 }, (_, i) => {
        const hour = i * 2;
        return `${hour.toString().padStart(2, "0")}:00`;
      });

      chartData.value = timeSlots.map((time, idx) => {
        const point = dataPoints[idx] || {};
        return {
          time,
          temperature: point.temperature ?? 0,
          humidity: point.humidity ?? 0,
          moisture: point.moisture ?? 0,
        };
      });
    } else {
      chartData.value = Array.from({ length: 12 }, (_, i) => {
        const hour = i * 2;
        return {
          time: `${hour.toString().padStart(2, "0")}:00`,
          temperature: 0,
          humidity: 0,
          moisture: 0,
        };
      });
    }
  } finally {
    loading.value = false;
  }
});
</script>

<template>
  <!-- Spacer for header height -->
  <div class="h-[95px]"></div>

  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar
      :isCollapsed="isSidebarCollapsed"
      @updateCollapsed="isSidebarCollapsed = $event"
    />

    <div
      class="flex-1 transition-all duration-300 ease-in-out max-md:ml-0"
      :class="isSidebarCollapsed ? 'md:ml-16' : 'md:ml-64'"
    >
      <Header @toggleSidebar="toggleSidebar" />
      <main class="p-4 sm:p-6 lg:p-8">
        <div class="max-w-7xl mx-auto">
          <!-- Sensor Cards -->
          <div
            class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6 mb-6 sm:mb-8"
          >
            <SensorCard
              label="suhu"
              :value="dryerStore.sensorData.suhu"
              unit="°C"
            />
            <SensorCard
              label="Kelembapan"
              :value="dryerStore.sensorData.humidity"
              unit="%"
            />
            <SensorCard
              label="Kadar Air"
              :value="dryerStore.sensorData.kadarAir"
              unit=""
            />
            <SensorCard
              label="Proses pengeringan"
              :value="dryerStore.statusData.pengeringan"
              unit=""
            />
            <StatusCard :status="dryerStore.overallStatus" />
          </div>

          <!-- Chart -->
          <div
            class="mt-6 sm:mt-8 bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
          >
            <ChartMonitoring
              :labels="chartData.map((d) => d.time)"
              :datasets="[
                {
                  label: 'Suhu (°C)',
                  data: chartData.map((d) => d.temperature),
                  borderColor: '#f87171',
                },
                {
                  label: 'Kelembapan (%)',
                  data: chartData.map((d) => d.humidity),
                  borderColor: '#60a5fa',
                },
                {
                  label: 'Kadar Air (%)',
                  data: chartData.map((d) => d.moisture),
                  borderColor: '#34d399',
                },
              ]"
              :isActive="dryerStore.systemStatus.process_status === 'running'"
            />
          </div>

          <!-- Information -->
          <div
            class="mt-6 sm:mt-8 bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
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
                  <p class="text-gray-600 dark:text-gray-400">
                    Koneksi Jaringan:
                  </p>
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
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
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
                  >
                    {{ wifiSSID }}
                  </p>
                </div>
              </div>

              <!-- Kekuatan sinyal WiFi -->
              <div class="flex items-start gap-3">
                <Signal class="w-5 h-5 text-yellow-500 mt-0.5 flex-shrink-0" />
                <div class="min-w-0">
                  <p class="text-gray-600 dark:text-gray-400">
                    Kekuatan Sinyal WiFi:
                  </p>
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
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
                  >
                    {{ ipAddress }}
                  </p>
                </div>
              </div>

              <!-- Sensor DHT22 -->
              <div class="flex items-start gap-3">
                <Thermometer
                  class="w-5 h-5 text-red-500 mt-0.5 flex-shrink-0"
                />
                <div class="min-w-0">
                  <p class="text-gray-600 dark:text-gray-400">
                    Status Sensor DHT22:
                  </p>
                  <p class="font-semibold text-gray-800 dark:text-white">
                    {{ sensorDHT22 }}
                  </p>
                </div>
              </div>

              <!-- Sensor HX711 -->
              <div class="flex items-start gap-3">
                <Gauge class="w-5 h-5 text-green-600 mt-0.5 flex-shrink-0" />
                <div class="min-w-0">
                  <p class="text-gray-600 dark:text-gray-400">
                    Status Sensor HX711:
                  </p>
                  <p class="font-semibold text-gray-800 dark:text-white">
                    {{ sensorHX711 }}
                  </p>
                </div>
              </div>

              <!-- Error Status -->
              <div
                class="flex items-start gap-3 col-span-1 sm:col-span-2 lg:col-span-4"
              >
                <AlertCircle
                  class="w-5 h-5 text-red-500 mt-0.5 flex-shrink-0"
                />
                <div class="min-w-0 flex-1">
                  <p class="text-gray-600 dark:text-gray-400">Status Error:</p>
                  <p
                    class="font-semibold text-gray-800 dark:text-white break-words"
                  >
                    {{ errorStatus }}
                  </p>
                </div>
              </div>
            </div>
          </div>
        </div>
      </main>
    </div>
  </div>
</template>
