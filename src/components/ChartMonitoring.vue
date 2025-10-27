<template>
  <div class="w-full h-[400px] relative">
    <Line v-if="showChart" :data="chartData" :options="chartOptions" />
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
  TimeScale
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

// tampilkan chart hanya jika ada data dan sesi aktif
const showChart = computed(() => props.isActive && props.datasets.length > 0);

// Hitung batas bawah dan atas sumbu Y
const yMin = computed(() => 0); // mulai dari 0
const yMax = computed(() => {
  const all = props.datasets.flatMap((d) => d.data);
  const maxVal = Math.max(...all);
  return maxVal + 10; // tambahkan 10 poin di atas data maksimum
});

// Data ChartJS
const chartData = computed(() => ({
  labels: props.labels,
  datasets: props.datasets.map((d) => ({
    ...d,
    borderWidth: 2,
    tension: 0.35,
    pointRadius: 0,
    fill: true,
    backgroundColor: d.backgroundColor || `${d.borderColor}33`, // efek transparan lembut
  })),
}));

// Format waktu di sumbu X
const timeDisplayFormats: Record<string, string> = {
  minute: "HH:mm",
  hour: "HH:mm",
  day: "MMM d",
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
      mode: "nearest",
      callbacks: {
        title: (context) => `Waktu: ${context[0].label}`,
        label: (context) =>
          `${context.dataset.label}: ${context.formattedValue}`,
      },
    },
    legend: {
      position: "bottom",
      labels: {
        usePointStyle: true,
        boxWidth: 10,
      },
    },
    title: {
      display: true,
      text: "Grafik Monitoring Proses Pengeringan",
      font: { size: 16 },
    },
  },
  scales: {
    x: {
      type: "time",
      time: {
        unit: props.timeframe || "minute",
        displayFormats: {
          [props.timeframe || "minute"]:
            timeDisplayFormats[props.timeframe || "minute"],
        },
      },
      title: {
        display: true,
        text: "Linimasa Pengeringan",
      },
      ticks: {
        autoSkip: true,
        maxTicksLimit: 10,
      },
      grid: {
        color: "rgba(255,255,255,0.05)",
      },
    },
    y: {
      beginAtZero: true,
      min: yMin.value,
      max: yMax.value,
      title: {
        display: true,
        text: "Nilai Sensor",
      },
      grid: {
        color: "rgba(255,255,255,0.05)",
      },
    },
  },
  elements: {
    line: {
      borderJoinStyle: "round",
      capBezierPoints: true,
    },
  },
}));

// Reaktifkan chart jika data berubah
watch([() => props.datasets, () => props.labels], () => {});
</script>
