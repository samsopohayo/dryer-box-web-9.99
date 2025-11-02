<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-all duration-300 hover:shadow-xl hover:scale-[1.02] border border-transparent dark:border-gray-700"
  >
    <div class="flex items-center justify-between">
      <div>
        <p class="text-sm text-gray-600 dark:text-gray-400 mb-1">{{ label }}</p>
        <p
          class="text-3xl font-bold text-gray-800 dark:text-white transition-colors duration-300"
        >
          {{ displayValue }}
        </p>
      </div>
      <div
        class="w-16 h-16 rounded-xl flex items-center justify-center transition-all duration-300"
        :class="iconBackgroundClass"
      >
        <component
          :is="sensorIcon"
          :class="iconColorClass"
          class="transition-all duration-300"
        />
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed, h } from "vue";

const props = defineProps<{
  label: string;
  value: number | string;
  unit: string;
}>();

const displayValue = computed(() => {
  if (typeof props.value === "number") {
    return `${props.value.toFixed(1)}${props.unit ?? ""}`;
  }
  return `${props.value}${props.unit ?? ""}`;
});

// Icon background with gradient based on label
const iconBackgroundClass = computed(() => {
  const labelLower = props.label.toLowerCase();
  if (labelLower.includes("suhu") || labelLower.includes("temp"))
    return "bg-gradient-to-br from-red-100 to-red-200 dark:from-red-900 dark:to-red-800";
  if (labelLower.includes("kelembapan") || labelLower.includes("humidity"))
    return "bg-gradient-to-br from-blue-100 to-blue-200 dark:from-blue-900 dark:to-blue-800";
  if (labelLower.includes("kadar") || labelLower.includes("moisture"))
    return "bg-gradient-to-br from-green-100 to-green-200 dark:from-green-900 dark:to-green-800";
  if (labelLower.includes("berat") || labelLower.includes("weight"))
    return "bg-gradient-to-br from-purple-100 to-purple-200 dark:from-purple-900 dark:to-purple-800";
  if (labelLower.includes("proses") || labelLower.includes("status"))
    return "bg-gradient-to-br from-indigo-100 to-indigo-200 dark:from-indigo-900 dark:to-indigo-800";
  return "bg-gradient-to-br from-gray-200 to-gray-300 dark:from-gray-700 dark:to-gray-600";
});

// Icon color class
const iconColorClass = computed(() => {
  const labelLower = props.label.toLowerCase();
  if (labelLower.includes("suhu") || labelLower.includes("temp"))
    return "text-red-600 dark:text-red-300 w-8 h-8";
  if (labelLower.includes("kelembapan") || labelLower.includes("humidity"))
    return "text-blue-600 dark:text-blue-300 w-8 h-8";
  if (labelLower.includes("kadar") || labelLower.includes("moisture"))
    return "text-green-600 dark:text-green-300 w-8 h-8";
  if (labelLower.includes("berat") || labelLower.includes("weight"))
    return "text-purple-600 dark:text-purple-300 w-8 h-8";
  if (labelLower.includes("proses") || labelLower.includes("status"))
    return "text-indigo-600 dark:text-indigo-300 w-8 h-8";
  return "text-gray-600 dark:text-gray-300 w-8 h-8";
});

// Sensor icon component
const sensorIcon = computed(() => {
  const labelLower = props.label.toLowerCase();

  // Temperature icon (thermometer)
  if (labelLower.includes("suhu") || labelLower.includes("temp")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-8 h-8",
      },
      [
        h("path", {
          d: "M15 13V5A3 3 0 0 0 9 5V13A5 5 0 1 0 15 13M12 4A1 1 0 0 1 13 5V8H11V5A1 1 0 0 1 12 4Z",
        }),
      ]
    );
  }

  // Humidity/Kelembapan icon (water drops + thermometer)
  if (labelLower.includes("kelembapan") || labelLower.includes("humidity")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-8 h-8",
      },
      [
        h("path", {
          d: "M12,3.77L11.25,4.61C11.25,4.61 9.97,6.06 8.68,7.94C7.39,9.82 6,12.07 6,14.23A6,6 0 0,0 12,20.23A6,6 0 0,0 18,14.23C18,12.07 16.61,9.82 15.32,7.94C14.03,6.06 12.75,4.61 12.75,4.61L12,3.77M12,6.9C12.44,7.42 12.84,7.85 13.68,9.07C14.89,10.83 16,13.07 16,14.23C16,16.45 14.22,18.23 12,18.23C9.78,18.23 8,16.45 8,14.23C8,13.07 9.11,10.83 10.32,9.07C11.16,7.85 11.56,7.42 12,6.9Z",
        }),
      ]
    );
  }

  // Moisture/Kadar Air icon (water drop + percentage)
  if (labelLower.includes("kadar") || labelLower.includes("moisture")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-8 h-8",
      },
      [
        h("path", {
          d: "M12,2.69L11.5,3.24L11,3.79L10.5,4.34C8.7,6.14 6,9.25 6,12.8C6,16.85 9.21,20 13,20C16.79,20 20,16.85 20,12.8C20,9.25 17.3,6.14 15.5,4.34L15,3.79L14.5,3.24L14,2.69L13,3.84L12,2.69M13,4.8L13.62,5.5C15.06,7.03 18,10.24 18,12.8C18,15.68 15.65,18 13,18C10.35,18 8,15.68 8,12.8C8,10.24 10.94,7.03 12.38,5.5L13,4.8M7.46,6C7.15,6.5 6.88,7 6.63,7.5L8.06,8.24C8.24,7.89 8.43,7.54 8.64,7.18L7.46,6M4.24,9.68C4.09,10.11 4,10.54 3.9,11H5.69C5.77,10.59 5.87,10.18 6,9.77L4.24,9.68M3.5,13V14H5.5V13H3.5M5.66,16.36L6.5,17.2C6.72,16.89 6.97,16.59 7.23,16.32L6.33,15.44C6.08,15.74 5.86,16.05 5.66,16.36Z",
        }),
      ]
    );
  }

  // Weight/Berat icon (scale)
  if (labelLower.includes("berat") || labelLower.includes("weight")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-8 h-8",
      },
      [
        h("path", {
          d: "M12,3A4,4 0 0,1 16,7C16,7.73 15.81,8.41 15.46,9H18C18.95,9 19.75,9.67 19.95,10.56C21.96,18.57 22,18.78 22,19A2,2 0 0,1 20,21H4A2,2 0 0,1 2,19C2,18.78 2.04,18.57 4.05,10.56C4.25,9.67 5.05,9 6,9H8.54C8.19,8.41 8,7.73 8,7A4,4 0 0,1 12,3M12,5A2,2 0 0,0 10,7A2,2 0 0,0 12,9A2,2 0 0,0 14,7A2,2 0 0,0 12,5M6,11L4,19H20L18,11H15.31L14.5,12.5L12,11.2L9.5,12.5L8.69,11H6Z",
        }),
      ]
    );
  }

  // Process/Proses icon (cog/settings)
  if (labelLower.includes("proses") || labelLower.includes("status")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-8 h-8",
      },
      [
        h("path", {
          d: "M12,15.5A3.5,3.5 0 0,1 8.5,12A3.5,3.5 0 0,1 12,8.5A3.5,3.5 0 0,1 15.5,12A3.5,3.5 0 0,1 12,15.5M19.43,12.97C19.47,12.65 19.5,12.33 19.5,12C19.5,11.67 19.47,11.34 19.43,11L21.54,9.37C21.73,9.22 21.78,8.95 21.66,8.73L19.66,5.27C19.54,5.05 19.27,4.96 19.05,5.05L16.56,6.05C16.04,5.66 15.5,5.32 14.87,5.07L14.5,2.42C14.46,2.18 14.25,2 14,2H10C9.75,2 9.54,2.18 9.5,2.42L9.13,5.07C8.5,5.32 7.96,5.66 7.44,6.05L4.95,5.05C4.73,4.96 4.46,5.05 4.34,5.27L2.34,8.73C2.21,8.95 2.27,9.22 2.46,9.37L4.57,11C4.53,11.34 4.5,11.67 4.5,12C4.5,12.33 4.53,12.65 4.57,12.97L2.46,14.63C2.27,14.78 2.21,15.05 2.34,15.27L4.34,18.73C4.46,18.95 4.73,19.03 4.95,18.95L7.44,17.94C7.96,18.34 8.5,18.68 9.13,18.93L9.5,21.58C9.54,21.82 9.75,22 10,22H14C14.25,22 14.46,21.82 14.5,21.58L14.87,18.93C15.5,18.67 16.04,18.34 16.56,17.94L19.05,18.95C19.27,19.03 19.54,18.95 19.66,18.73L21.66,15.27C21.78,15.05 21.73,14.78 21.54,14.63L19.43,12.97Z",
        }),
      ]
    );
  }

  // Default icon (chart)
  return h(
    "svg",
    {
      fill: "none",
      stroke: "currentColor",
      viewBox: "0 0 24 24",
      class: "w-8 h-8",
    },
    [
      h("path", {
        "stroke-linecap": "round",
        "stroke-linejoin": "round",
        "stroke-width": "2",
        d: "M9 19v-6a2 2 0 00-2-2H5a2 2 0 00-2 2v6a2 2 0 002 2h2a2 2 0 002-2zm0 0V9a2 2 0 012-2h2a2 2 0 012 2v10m-6 0a2 2 0 002 2h2a2 2 0 002-2m0 0V5a2 2 0 012-2h2a2 2 0 012 2v14a2 2 0 01-2 2h-2a2 2 0 01-2-2z",
      }),
    ]
  );
});
</script>
