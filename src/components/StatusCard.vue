<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-all duration-300 hover:shadow-xl hover:scale-[1.02] border border-transparent dark:border-gray-700 relative overflow-hidden"
  >
    <div class="flex items-center justify-between">
      <div>
        <p class="text-sm text-gray-600 dark:text-gray-400 mb-1">Status</p>
        <p
          class="text-3xl font-bold transition-colors duration-300"
          :class="offline ? 'text-gray-400 dark:text-gray-600' : statusColor"
        >
          {{ offline ? "Offline" : status }}
        </p>
      </div>
      <div
        class="w-16 h-16 rounded-xl flex items-center justify-center transition-all duration-300"
        :class="iconBackgroundClass"
      >
        <component
          :is="statusIcon"
          :class="iconColorClass"
          class="transition-all duration-300"
        />
      </div>
    </div>

    <!-- Offline indicator bar -->
    <div
      v-if="offline"
      class="absolute bottom-0 left-0 right-0 h-1 bg-red-500"
    ></div>
  </div>
</template>

<script setup lang="ts">
import { computed, h } from "vue";

const props = defineProps<{
  status: string;
  offline?: boolean;
}>();

// Status color
const statusColor = computed(() => {
  if (props.offline) return "text-gray-400 dark:text-gray-600";

  const statusLower = props.status.toLowerCase();
  if (statusLower.includes("kering"))
    return "text-green-600 dark:text-green-400";
  if (statusLower.includes("normal")) return "text-blue-600 dark:text-blue-400";
  if (statusLower.includes("lembab"))
    return "text-yellow-600 dark:text-yellow-400";
  if (statusLower.includes("proteksi")) return "text-red-600 dark:text-red-400";
  if (statusLower.includes("terbuka"))
    return "text-orange-600 dark:text-orange-400";
  return "text-gray-800 dark:text-gray-200";
});

// Icon background class with gradient
const iconBackgroundClass = computed(() => {
  if (props.offline) {
    return "bg-gray-200 dark:bg-gray-700 opacity-50";
  }

  const statusLower = props.status.toLowerCase();
  if (statusLower.includes("kering"))
    return "bg-gradient-to-br from-green-100 to-green-200 dark:from-green-900 dark:to-green-800";
  if (statusLower.includes("normal"))
    return "bg-gradient-to-br from-blue-100 to-blue-200 dark:from-blue-900 dark:to-blue-800";
  if (statusLower.includes("lembab"))
    return "bg-gradient-to-br from-yellow-100 to-yellow-200 dark:from-yellow-900 dark:to-yellow-800";
  if (statusLower.includes("proteksi"))
    return "bg-gradient-to-br from-red-100 to-red-200 dark:from-red-900 dark:to-red-800";
  if (statusLower.includes("terbuka"))
    return "bg-gradient-to-br from-orange-100 to-orange-200 dark:from-orange-900 dark:to-orange-800";
  return "bg-gradient-to-br from-gray-200 to-gray-300 dark:from-gray-700 dark:to-gray-600";
});

// Icon color class
const iconColorClass = computed(() => {
  if (props.offline) {
    return "text-gray-400 dark:text-gray-600 w-8 h-8";
  }

  const statusLower = props.status.toLowerCase();
  if (statusLower.includes("kering"))
    return "text-green-600 dark:text-green-300 w-8 h-8";
  if (statusLower.includes("normal"))
    return "text-blue-600 dark:text-blue-300 w-8 h-8";
  if (statusLower.includes("lembab"))
    return "text-yellow-600 dark:text-yellow-300 w-8 h-8";
  if (statusLower.includes("proteksi"))
    return "text-red-600 dark:text-red-300 w-8 h-8";
  if (statusLower.includes("terbuka"))
    return "text-orange-600 dark:text-orange-300 w-8 h-8";
  return "text-gray-600 dark:text-gray-300 w-8 h-8";
});

// Status icon component
const statusIcon = computed(() => {
  if (props.offline) {
    // Offline icon
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
          d: "M18.364 5.636a9 9 0 010 12.728m0 0l-2.829-2.829m2.829 2.829L21 21M15.536 8.464a5 5 0 010 7.072m0 0l-2.829-2.829m-4.243 2.829a4.978 4.978 0 01-1.414-2.83m-1.414 5.658a9 9 0 01-2.167-9.238m7.824 2.167a1 1 0 111.414 1.414m-1.414-1.414L3 3m8.293 8.293l1.414 1.414",
        }),
      ]
    );
  }

  const statusLower = props.status.toLowerCase();

  // Kering - Check icon
  if (statusLower.includes("kering")) {
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
          d: "M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z",
        }),
      ]
    );
  }

  // Normal - Smile icon
  if (statusLower.includes("normal")) {
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
          d: "M14.828 14.828a4 4 0 01-5.656 0M9 10h.01M15 10h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z",
        }),
      ]
    );
  }

  // Lembab - Droplet icon
  if (statusLower.includes("lembab")) {
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
          d: "M7 16a4 4 0 01-.88-7.903A5 5 0 1115.9 6L16 6a5 5 0 011 9.9M9 19l3 3m0 0l3-3m-3 3V10",
        }),
      ]
    );
  }

  // Proteksi - Alert icon
  if (statusLower.includes("proteksi")) {
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
          d: "M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z",
        }),
      ]
    );
  }

  // Terbuka - Door Open icon
  if (statusLower.includes("terbuka")) {
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
          d: "M8 11V7a4 4 0 118 0m-4 8v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2z",
        }),
      ]
    );
  }

  // Default - Info icon
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
        d: "M13 16h-1v-4h-1m1-4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z",
      }),
    ]
  );
});
</script>
