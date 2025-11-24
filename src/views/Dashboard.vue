<template>
  <FloatingQnA />
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
          <!-- Connection Status Banner -->
          <div
            v-if="!isConnected"
            class="mb-6 bg-gradient-to-r from-red-500 to-red-600 rounded-xl shadow-lg p-4 flex items-center space-x-4 animate-pulse"
          >
            <div
              class="w-12 h-12 bg-white rounded-full flex items-center justify-center"
            >
              <svg
                class="w-6 h-6 text-red-600 animate-pulse"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  stroke-width="2"
                  d="M18.364 5.636a9 9 0 010 12.728m0 0l-2.829-2.829m2.829 2.829L21 21M15.536 8.464a5 5 0 010 7.072m0 0l-2.829-2.829m-4.243 2.829a4.978 4.978 0 01-1.414-2.83m-1.414 5.658a9 9 0 01-2.167-9.238m7.824 2.167a1 1 0 111.414 1.414m-1.414-1.414L3 3m8.293 8.293l1.414 1.414"
                />
              </svg>
            </div>
            <div class="flex-1">
              <p class="font-bold text-white text-lg">
                ⚠️ SISTEM TIDAK TERHUBUNG
              </p>
              <p class="text-white text-sm">
                Data monitoring tidak dapat diperbarui. Koneksi terakhir:
                {{ lastUpdateText }}
              </p>
            </div>
          </div>

          <!-- Weather & System Status Card -->
          <div
            v-if="isConnected"
            class="mb-6 bg-gradient-to-r from-blue-500 to-indigo-600 dark:from-blue-600 dark:to-indigo-700 rounded-xl shadow-lg p-4 sm:p-6"
          >
            <div class="flex items-center justify-between flex-wrap gap-4">
              <!-- Weather Info -->
              <div class="flex items-center space-x-4">
                <div class="flex items-center space-x-2">
                  <svg
                    class="w-5 h-5 text-white"
                    :class="collectorActive ? 'animate-spin' : ''"
                    fill="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      d="M12,11A1,1 0 0,0 11,12A1,1 0 0,0 12,13A1,1 0 0,0 13,12A1,1 0 0,0 12,11M12.5,2C17,2 17.11,5.57 14.75,6.75C13.76,7.24 13.32,8.29 13.13,9.22C13.61,9.42 14.03,9.73 14.35,10.13C18.05,8.13 22.03,8.92 22.03,12.5C22.03,17 18.46,17.1 17.28,14.73C16.78,13.74 15.72,13.3 14.79,13.11C14.59,13.59 14.28,14 13.88,14.34C15.87,18.03 15.08,22 11.5,22C7,22 6.91,18.42 9.27,17.24C10.25,16.75 10.69,15.71 10.89,14.79C10.4,14.59 9.97,14.27 9.65,13.87C5.96,15.85 2,15.07 2,11.5C2,7 5.56,6.89 6.74,9.26C7.24,10.25 8.29,10.68 9.22,10.87C9.41,10.39 9.73,9.97 10.14,9.65C8.15,5.96 8.94,2 12.5,2Z"
                    />
                  </svg>
                  <p class="text-white text-sm">
                    <strong>Fan Collector:</strong>
                    {{ collectorStatusText }}
                  </p>
                </div>
              </div>

              <!-- System Mode & Control Info -->
              <div class="flex items-center space-x-6">
                <div class="text-right">
                  <p class="text-white text-sm opacity-90">Mode Kontrol</p>
                  <div class="flex items-center space-x-2 mt-1">
                    <span
                      class="px-3 py-1 rounded-full text-sm font-semibold"
                      :class="
                        isAutoMode
                          ? 'bg-white text-blue-600'
                          : 'bg-yellow-400 text-gray-800'
                      "
                    >
                      {{ isAutoMode ? "AUTO" : "MANUAL" }}
                    </span>
                    <div
                      v-if="isAutoMode"
                      class="w-2 h-2 bg-green-400 rounded-full animate-pulse"
                    ></div>
                  </div>
                </div>
              </div>
            </div>
          </div>

          <!-- Sensor Cards -->
          <div
            class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6 mb-6 sm:mb-8"
          >
            <SensorCard
              label="Suhu"
              :value="dryerStore.sensorData.suhu"
              unit="°C"
              :offline="!isConnected"
            />
            <SensorCard
              label="Kelembapan"
              :value="dryerStore.sensorData.humidity"
              unit="%"
              :offline="!isConnected"
            />
            <SensorCard
              label="Kadar Air"
              :value="dryerStore.sensorData.kadarAir"
              unit="%"
              :offline="!isConnected"
            />
            <SensorCard
              label="Berat"
              :value="dryerStore.sensorData.berat"
              unit=" g"
              :offline="!isConnected"
            />
            <StatusCard
              :status="dryerStore.overallStatus"
              :offline="!isConnected"
            />
            <SensorCard
              label="Status Proses"
              :value="dryerStore.statusData.pengeringan"
              unit=""
              :offline="!isConnected"
            />
          </div>

          <!-- Chart -->
          <div
            class="mt-6 sm:mt-8 bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors relative"
          >
            <ChartMonitoring />
          </div>

          <!-- System Information -->
          <InformationCard />
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useDryerStore } from "@/stores/dryer";
import { useLocationStore } from "@/stores/location";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import InformationCard from "@/components/InformationCard.vue";
import SensorCard from "@/components/SensorCard.vue";
import StatusCard from "@/components/StatusCard.vue";
import ChartMonitoring from "@/components/ChartMonitoring.vue";
import FloatingQnA from "@/components/FloatingQnA.vue";

const dryerStore = useDryerStore();
const locationStore = useLocationStore();
const isSidebarCollapsed = ref(true);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

// Check if system is connected
const isConnected = computed(() => {
  const lastUpdate = dryerStore.systemStatus.last_update;
  if (!lastUpdate) return false;

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

// Last update text
const lastUpdateText = computed(() => {
  const lastUpdate = dryerStore.systemStatus.last_update;
  if (!lastUpdate) return "Tidak tersedia";
  return lastUpdate;
});

// Control mode
const isAutoMode = computed(() => {
  return dryerStore.controlData.auto_mode !== false;
});

const collectorActive = computed(() => {
  const condition = locationStore.weather.condition;
  const isSunny = condition === "cerah" || condition === "berawan";

  const now = new Date();
  const hour = now.getHours();
  const isCollectorTime = hour >= 6 && hour < 17;

  return isSunny && isCollectorTime;
});

const collectorStatusText = computed(() => {
  if (collectorActive.value) {
    return "AKTIF (Cuaca cerah & jam operasi 06:00-17:00)";
  }

  const condition = locationStore.weather.condition;
  const now = new Date();
  const hour = now.getHours();
  const isCollectorTime = hour >= 6 && hour < 17;

  if (!isCollectorTime) {
    return "MATI (Di luar jam operasi 06:00-17:00)";
  }

  return `MATI (Cuaca ${condition})`;
});
</script>
