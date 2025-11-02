<template>
  <div class="w-full h-[400px] relative">
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
          {{ isActive ? "Menunggu data sensor..." : "Sistem belum dimulai" }}
        </p>
      </div>
    </div>
    <Line v-else :data="chartData" :options="chartOptions" />
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
import { computed, watch } from "vue";

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

const props = defineProps<{
  labels: string[];
  datasets: {
    label: string;
    data: number[];
    borderColor?: string;
    backgroundColor?: string;
  }[];
  timeframe?: "minute" | "hour" | "day" | "week";
  isActive?: boolean;
}>();

// Tampilkan chart hanya jika ada data yang valid
const showChart = computed(() => {
  // Cek apakah ada dataset dengan data yang tidak kosong
  const hasValidData = props.datasets.some((dataset) => {
    return dataset.data.some((value) => value > 0);
  });

  return hasValidData && props.labels.length > 0;
});

// Hitung batas bawah dan atas sumbu Y
const yMin = computed(() => 0);
const yMax = computed(() => {
  const allValues = props.datasets.flatMap((d) => d.data);
  const maxVal = Math.max(...allValues.filter((v) => v > 0));

  // Jika tidak ada data valid, return 100 sebagai default
  if (!isFinite(maxVal) || maxVal === 0) return 100;

  // Tambahkan 10% padding di atas nilai maksimum
  return Math.ceil(maxVal * 1.1);
});

// Data ChartJS
const chartData = computed(() => ({
  labels: props.labels,
  datasets: props.datasets.map((d) => ({
    ...d,
    borderWidth: 2,
    tension: 0.4,
    pointRadius: 3,
    pointHoverRadius: 5,
    fill: true,
    backgroundColor: d.backgroundColor || `${d.borderColor}33`,
  })),
}));

// Format waktu di sumbu X
const timeDisplayFormats: Record<string, string> = {
  minute: "HH:mm",
  hour: "HH:mm",
  day: "dd MMM",
  week: "'Week' w",
};

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

// Log untuk debugging
watch(
  [() => props.datasets, () => props.labels, () => showChart.value],
  ([datasets, labels, show]) => {
    console.log("Chart Update:", {
      showChart: show,
      labelsCount: labels.length,
      datasetsCount: datasets.length,
      firstDataset: datasets[0],
      isActive: props.isActive,
    });
  },
  { deep: true }
);
</script>
