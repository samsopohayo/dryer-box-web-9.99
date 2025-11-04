<script setup lang="ts">
import { computed, onMounted, ref, watch, onBeforeUnmount } from "vue";
import { useDryerStore } from "@/stores/dryer";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import InformationCard from "@/components/InformationCard.vue";
import SensorCard from "@/components/SensorCard.vue";
import StatusCard from "@/components/StatusCard.vue";
import ChartMonitoring from "@/components/ChartMonitoring.vue";

const dryerStore = useDryerStore();
const chartData = ref<any[]>([]);
const loading = ref(true);
const isSidebarCollapsed = ref(true);
const processStartTime = ref<number | null>(null);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const networkStatus = computed(() => {
  return dryerStore.systemStatus.status === "ready" ? "Stabil" : "Tidak Stabil";
});

const wifiSSID = computed(() => dryerStore.systemStatus.wifi_ssid || "-");

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

// Format elapsed time dari menit ke HH:MM
const formatElapsedTime = (minutes: number): string => {
  const hrs = Math.floor(minutes / 60);
  const mins = minutes % 60;
  return `${hrs.toString().padStart(2, "0")}:${mins
    .toString()
    .padStart(2, "0")}`;
};

// Parse timestamp "DD/MM/YYYY HH:MM:SS" ke Date object
const parseTimestamp = (timestamp: string): Date => {
  const [date, time] = timestamp.split(" ");
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

// Fungsi untuk memuat data dari session aktif
const loadSessionData = async () => {
  loading.value = true;
  try {
    const currentSession = dryerStore.systemStatus.current_session;

    if (currentSession && currentSession !== "") {
      console.log("Loading session data for:", currentSession);

      await dryerStore.fetchSessionData(currentSession);

      const dataPoints: any[] = Object.values(
        dryerStore.currentSessionData || {}
      );

      console.log("Raw data points fetched:", dataPoints.length);

      if (dataPoints.length > 0) {
        // Sort by timestamp
        dataPoints.sort((a, b) => {
          return (
            parseTimestamp(a.timestamp).getTime() -
            parseTimestamp(b.timestamp).getTime()
          );
        });

        // Ambil waktu mulai (data pertama)
        const startTime = parseTimestamp(dataPoints[0].timestamp).getTime();
        processStartTime.value = startTime;

        // Convert semua data ke format chart dengan elapsed time
        const chartPoints = dataPoints.map((point) => {
          const currentTime = parseTimestamp(point.timestamp).getTime();
          const elapsedMinutes = Math.floor((currentTime - startTime) / 60000);

          return {
            time: formatElapsedTime(elapsedMinutes),
            temperature: point.temperature || 0,
            humidity: point.humidity || 0,
            moisture: point.moisture || 0,
            elapsedMinutes: elapsedMinutes,
          };
        });

        // Ambil maksimal 50 data point terakhir untuk performa
        chartData.value = chartPoints.slice(-50);

        console.log("Chart data prepared:", chartData.value.length, "points");
        console.log("First point:", chartData.value[0]);
        console.log("Last point:", chartData.value[chartData.value.length - 1]);
      } else {
        console.log("No data points found in session");
        chartData.value = [];
        processStartTime.value = null;
      }
    } else {
      console.log("No active session");
      // Jika tidak ada session, gunakan data sensor real-time
      useRealtimeData();
    }
  } catch (error) {
    console.error("Error loading session data:", error);
    // Fallback ke data real-time jika ada error
    useRealtimeData();
  } finally {
    loading.value = false;
  }
};

// Fungsi untuk menggunakan data sensor real-time (ketika belum ada session)
const useRealtimeData = () => {
  console.log("Using real-time sensor data");

  // Buat data dummy untuk 10 menit terakhir
  const now = Date.now();
  const points = [];

  for (let i = 0; i < 10; i++) {
    const elapsedMinutes = i;
    points.push({
      time: formatElapsedTime(elapsedMinutes),
      temperature: i === 9 ? dryerStore.sensorData.suhu : 0,
      humidity: i === 9 ? dryerStore.sensorData.humidity : 0,
      moisture: i === 9 ? dryerStore.sensorData.kadarAir : 0,
      elapsedMinutes: elapsedMinutes,
    });
  }

  chartData.value = points;
  processStartTime.value = now - 9 * 60000; // 9 menit yang lalu
};

// Computed untuk chart labels dan datasets
const chartLabels = computed(() => {
  if (chartData.value.length === 0) {
    // Default labels jika tidak ada data
    return Array.from({ length: 10 }, (_, i) => formatElapsedTime(i));
  }
  return chartData.value.map((d) => d.time);
});

const chartDatasets = computed(() => [
  {
    label: "Suhu (°C)",
    data: chartData.value.map((d) => d.temperature),
    borderColor: "#ef4444",
    backgroundColor: "rgba(239, 68, 68, 0.1)",
  },
  {
    label: "Kelembapan (%)",
    data: chartData.value.map((d) => d.humidity),
    borderColor: "#3b82f6",
    backgroundColor: "rgba(59, 130, 246, 0.1)",
  },
  {
    label: "Kadar Air (%)",
    data: chartData.value.map((d) => d.moisture),
    borderColor: "#10b981",
    backgroundColor: "rgba(16, 185, 129, 0.1)",
  },
]);

// Watch untuk perubahan pada sensor data real-time
watch(
  () => [
    dryerStore.sensorData.suhu,
    dryerStore.sensorData.humidity,
    dryerStore.sensorData.kadarAir,
  ],
  () => {
    // Update chart dengan data real-time jika tidak ada session aktif
    if (
      !dryerStore.systemStatus.current_session ||
      dryerStore.systemStatus.current_session === ""
    ) {
      useRealtimeData();
    }
  }
);

// Watch untuk perubahan session
watch(
  () => dryerStore.systemStatus.current_session,
  (newSession, oldSession) => {
    console.log("Session changed from", oldSession, "to", newSession);
    if (newSession !== oldSession) {
      loadSessionData();
    }
  }
);

// Watch untuk perubahan data session (setiap 15 detik)
watch(
  () => dryerStore.currentSessionData,
  (newData, oldData) => {
    // Hanya reload jika ada perubahan data yang signifikan
    const newCount = Object.keys(newData || {}).length;
    const oldCount = Object.keys(oldData || {}).length;

    if (newCount !== oldCount) {
      console.log("Session data count changed:", oldCount, "->", newCount);
      loadSessionData();
    }
  },
  { deep: true }
);

// Auto-refresh setiap 30 detik
let refreshInterval: number | null = null;

onMounted(async () => {
  await loadSessionData();

  // Set interval untuk refresh data
  refreshInterval = window.setInterval(() => {
    if (dryerStore.systemStatus.current_session) {
      console.log("Auto-refreshing chart data...");
      loadSessionData();
    }
  }, 30000); // Refresh setiap 30 detik
});

onBeforeUnmount(() => {
  if (refreshInterval) {
    clearInterval(refreshInterval);
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
              label="Suhu"
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
              unit="%"
            />
            <SensorCard
              label="Berat"
              :value="dryerStore.sensorData.berat"
              unit=" g"
            />
            <StatusCard :status="dryerStore.overallStatus" />
            <SensorCard
              label="Status Proses"
              :value="dryerStore.statusData.pengeringan"
              unit=""
            />
          </div>

          <!-- Chart -->
          <div
            class="mt-6 sm:mt-8 bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
          >
            <div
              v-if="dryerStore.systemStatus.current_session"
              class="mb-4 flex items-center justify-between"
            >
              <div class="text-sm text-gray-600 dark:text-gray-400">
                <span class="font-semibold">Session:</span>
                {{ dryerStore.systemStatus.current_session }}
              </div>
              <div class="text-sm text-gray-600 dark:text-gray-400">
                <span class="font-semibold">Data Points:</span>
                {{ chartData.length }}
              </div>
            </div>

            <div
              v-if="loading"
              class="flex items-center justify-center h-[400px]"
            >
              <div class="text-center">
                <div
                  class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary mb-4"
                ></div>
                <p class="text-gray-600 dark:text-gray-400">
                  Memuat data chart...
                </p>
              </div>
            </div>
            <ChartMonitoring
              v-else
              :labels="chartLabels"
              :datasets="chartDatasets"
              :isActive="dryerStore.systemStatus.process_status === 'running'"
            />
          </div>

          <!-- System Information -->
          <InformationCard />
        </div>
      </main>
    </div>
  </div>
</template>
