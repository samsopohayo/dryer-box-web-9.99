<template>
  <div>
    <!-- Session Info & Loading -->
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

    <div v-if="loading" class="flex items-center justify-center h-[400px]">
      <div class="text-center">
        <div
          class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary mb-4"
        ></div>
        <p class="text-gray-600 dark:text-gray-400">Memuat data chart...</p>
      </div>
    </div>

    <!-- Chart Container -->
    <div v-else class="w-full h-[400px] relative">
      <div v-if="!showChart" class="flex items-center justify-center h-full">
        <div class="text-center text-gray-500 dark:text-gray-400">
          <svg
            class="w-16 h-16 mx-auto mb-4"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z"
            />
          </svg>
          <p class="font-semibold">Tidak ada data monitoring</p>
          <p class="text-sm mt-2">
            {{
              dryerStore.systemStatus.process_status === "running"
                ? "Menunggu data sensor..."
                : "Sistem belum dimulai"
            }}
          </p>
        </div>
      </div>
      <Line v-else :data="chartDataFormatted" :options="chartOptions" />
    </div>
  </div>
</template>

<script lang="ts" setup>
import {
  Chart as ChartJS,
  Title,
  Tooltip,
  Legend,
  LineElement,
  PointElement,
  LinearScale,
  TimeScale,
  CategoryScale,
  ChartOptions,
  Filler,
} from "chart.js";
import "chartjs-adapter-date-fns";
import { Line } from "vue-chartjs";
import { computed, watch, ref, onMounted, onBeforeUnmount } from "vue";
import { useDryerStore } from "@/stores/dryer";

ChartJS.register(
  Title,
  Tooltip,
  Legend,
  LineElement,
  PointElement,
  LinearScale,
  CategoryScale,
  TimeScale,
  Filler
);

const dryerStore = useDryerStore();
const chartData = ref<any[]>([]);
const loading = ref(true);
const processStartTime = ref<number | null>(null);

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

// Computed untuk chart labels
const chartLabels = computed(() => {
  if (chartData.value.length === 0) {
    // Default labels jika tidak ada data
    return Array.from({ length: 10 }, (_, i) => formatElapsedTime(i));
  }
  return chartData.value.map((d) => d.time);
});

// Computed untuk chart datasets
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

// Tampilkan chart hanya jika ada data yang valid
const showChart = computed(() => {
  const hasValidData = chartDatasets.value.some((dataset) => {
    return dataset.data.some((value) => value > 0);
  });

  return hasValidData && chartLabels.value.length > 0;
});

// Hitung batas bawah dan atas sumbu Y
const yMin = computed(() => 0);
const yMax = computed(() => {
  const allValues = chartDatasets.value.flatMap((d) => d.data);
  const maxVal = Math.max(...allValues.filter((v) => v > 0));

  if (!isFinite(maxVal) || maxVal === 0) return 100;

  return Math.ceil(maxVal * 1.1);
});

// Data ChartJS
const chartDataFormatted = computed(() => ({
  labels: chartLabels.value,
  datasets: chartDatasets.value.map((d) => ({
    ...d,
    borderWidth: 2,
    tension: 0.4,
    pointRadius: 3,
    pointHoverRadius: 5,
    fill: true,
    backgroundColor: d.backgroundColor || `${d.borderColor}33`,
  })),
}));

// Opsi ChartJS
const chartOptions = computed<ChartOptions<"line">>(() => ({
  responsive: true,
  maintainAspectRatio: false,
  interaction: {
    mode: "index",
    intersect: false,
  },
  plugins: {
    tooltip: {
      enabled: true,
      mode: "index",
      intersect: false,
      callbacks: {
        title: (context) => {
          return `Waktu: ${context[0].label}`;
        },
        label: (context) => {
          const label = context.dataset.label || "";
          const value = context.parsed.y.toFixed(1);
          return `${label}: ${value}`;
        },
      },
    },
    legend: {
      position: "bottom",
      labels: {
        usePointStyle: true,
        boxWidth: 10,
        padding: 15,
        font: {
          size: 12,
        },
      },
    },
    title: {
      display: true,
      text: "Grafik Monitoring Proses Pengeringan",
      font: {
        size: 16,
        weight: "bold",
      },
      padding: {
        bottom: 20,
      },
    },
  },
  scales: {
    x: {
      type: "category",
      title: {
        display: true,
        text: "Waktu",
        font: {
          size: 12,
          weight: "bold",
        },
      },
      ticks: {
        autoSkip: true,
        maxTicksLimit: 12,
        maxRotation: 45,
        minRotation: 0,
      },
      grid: {
        color: "rgba(0, 0, 0, 0.05)",
        drawBorder: false,
      },
    },
    y: {
      beginAtZero: true,
      min: yMin.value,
      max: yMax.value,
      title: {
        display: true,
        text: "Nilai Sensor",
        font: {
          size: 12,
          weight: "bold",
        },
      },
      ticks: {
        stepSize: 10,
      },
      grid: {
        color: "rgba(0, 0, 0, 0.05)",
        drawBorder: false,
      },
    },
  },
  elements: {
    line: {
      borderJoinStyle: "round",
    },
    point: {
      hoverBorderWidth: 2,
    },
  },
}));

// Watch untuk perubahan pada sensor data real-time
watch(
  () => [
    dryerStore.sensorData.suhu,
    dryerStore.sensorData.humidity,
    dryerStore.sensorData.kadarAir,
  ],
  () => {
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

// Watch untuk perubahan data session
watch(
  () => dryerStore.currentSessionData,
  (newData, oldData) => {
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

  refreshInterval = window.setInterval(() => {
    if (dryerStore.systemStatus.current_session) {
      console.log("Auto-refreshing chart data...");
      loadSessionData();
    }
  }, 30000);
});

onBeforeUnmount(() => {
  if (refreshInterval) {
    clearInterval(refreshInterval);
  }
});
</script>
