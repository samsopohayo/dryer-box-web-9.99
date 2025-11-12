<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-all duration-300 hover:shadow-xl border border-transparent dark:border-gray-700"
  >
    <div class="flex items-center justify-between mb-4">
      <div class="flex items-center space-x-3">
        <div
          class="w-12 h-12 rounded-xl flex items-center justify-center transition-all duration-300"
          :class="iconBackgroundClass"
        >
          <component
            :is="deviceIcon"
            :class="iconColorClass"
            class="transition-all duration-300"
          />
        </div>
        <div>
          <h3 class="font-bold text-lg text-gray-800 dark:text-white">
            {{ title }}
          </h3>
          <p
            class="text-sm transition-colors duration-300"
            :class="statusClass"
          >
            status: {{ statusText }}
          </p>
        </div>
      </div>
    </div>

    <button
      @click="handleToggle"
      :disabled="!isConnected"
      class="w-full py-3 rounded-lg font-semibold transition-all duration-300 transform active:scale-95 relative overflow-hidden group disabled:opacity-50 disabled:cursor-not-allowed"
      :class="buttonClass"
    >
      <span class="relative z-10 flex items-center justify-center space-x-2">
        <component
          :is="buttonIcon"
          class="w-5 h-5 transition-transform duration-300 group-hover:scale-110"
        />
        <span>{{ buttonText }}</span>
      </span>
      <div
        class="absolute inset-0 bg-white opacity-0 group-hover:opacity-20 transition-opacity duration-300"
      ></div>
    </button>

    <!-- Connection Warning -->
    <div
      v-if="!isConnected"
      class="mt-3 p-2 bg-yellow-50 dark:bg-yellow-900 border border-yellow-200 dark:border-yellow-700 rounded-lg flex items-center gap-2"
    >
      <svg
        class="w-4 h-4 text-yellow-600 dark:text-yellow-400 flex-shrink-0"
        fill="none"
        stroke="currentColor"
        viewBox="0 0 24 24"
      >
        <path
          stroke-linecap="round"
          stroke-linejoin="round"
          stroke-width="2"
          d="M12 9v2m0 4h.01m-6.938 4h13.856c1.54 0 2.502-1.667 1.732-3L13.732 4c-.77-1.333-2.694-1.333-3.464 0L3.34 16c-.77 1.333.192 3 1.732 3z"
        />
      </svg>
      <span class="text-xs text-yellow-700 dark:text-yellow-300">
        Sistem tidak terkoneksi
      </span>
    </div>

    <!-- Device Info -->
    <div class="mt-3 p-2 bg-gray-50 dark:bg-gray-700 rounded-lg">
      <p class="text-xs text-gray-600 dark:text-gray-400">
        {{ deviceInfo }}
      </p>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed, h } from "vue";
import { useDryerStore } from "@/stores/dryer";

const props = defineProps<{
  title: string;
  device: "heater" | "fan_collector" | "fan_panel" | "exhaust";
}>();

const emit = defineEmits<{
  toggle: [];
}>();

const dryerStore = useDryerStore();

// Check if system is connected (last update within 1 minute)
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

// Get device state from store based on device prop
const isOn = computed(() => {
  switch (props.device) {
    case "heater":
      return dryerStore.controlData.manual_heater_state;
    case "fan_collector":
      return dryerStore.controlData.manual_fan_collector_state || false;
    case "fan_panel":
      return dryerStore.controlData.manual_fan_panel_state || false;
    case "exhaust":
      return dryerStore.controlData.manual_exhaust_state;
    default:
      return false;
  }
});

const statusText = computed(() => {
  if (!isConnected.value) return "Offline";
  return isOn.value ? "Hidup" : "Mati";
});

const statusClass = computed(() => {
  if (!isConnected.value) return "text-gray-500 dark:text-gray-400";
  return isOn.value
    ? "text-green-600 dark:text-green-400 font-semibold"
    : "text-gray-600 dark:text-gray-400";
});

const buttonClass = computed(() => {
  if (!isConnected.value) {
    return "bg-gray-400 text-white cursor-not-allowed";
  }

  return isOn.value
    ? "bg-gradient-to-r from-red-500 to-red-600 text-white hover:from-red-600 hover:to-red-700 shadow-lg shadow-red-500/50 dark:shadow-red-900/50"
    : "bg-gradient-to-r from-blue-500 to-blue-600 text-white hover:from-blue-600 hover:to-blue-700 shadow-lg shadow-blue-500/50 dark:shadow-blue-900/50";
});

const buttonText = computed(() => {
  if (!isConnected.value) return "Tidak Tersedia";
  return isOn.value ? "Matikan" : "Hidupkan";
});

// Icon background with gradient
const iconBackgroundClass = computed(() => {
  const titleLower = props.title.toLowerCase();

  if (!isConnected.value) {
    return "bg-gradient-to-br from-gray-300 to-gray-400 dark:from-gray-600 dark:to-gray-700";
  }

  if (titleLower.includes("heater"))
    return isOn.value
      ? "bg-gradient-to-br from-red-100 to-red-200 dark:from-red-900 dark:to-red-800"
      : "bg-gradient-to-br from-gray-100 to-gray-200 dark:from-gray-700 dark:to-gray-600";
  if (titleLower.includes("fan"))
    return isOn.value
      ? "bg-gradient-to-br from-blue-100 to-blue-200 dark:from-blue-900 dark:to-blue-800"
      : "bg-gradient-to-br from-gray-100 to-gray-200 dark:from-gray-700 dark:to-gray-600";
  if (titleLower.includes("exhaust"))
    return isOn.value
      ? "bg-gradient-to-br from-green-100 to-green-200 dark:from-green-900 dark:to-green-800"
      : "bg-gradient-to-br from-gray-100 to-gray-200 dark:from-gray-700 dark:to-gray-600";
  return "bg-gradient-to-br from-gray-100 to-gray-200 dark:from-gray-700 dark:to-gray-600";
});

// Icon color
const iconColorClass = computed(() => {
  const titleLower = props.title.toLowerCase();

  if (!isConnected.value) {
    return "text-gray-500 dark:text-gray-400";
  }

  if (titleLower.includes("heater"))
    return isOn.value
      ? "text-red-600 dark:text-red-300"
      : "text-gray-600 dark:text-gray-400";
  if (titleLower.includes("fan"))
    return isOn.value
      ? "text-blue-600 dark:text-blue-300"
      : "text-gray-600 dark:text-gray-400";
  if (titleLower.includes("exhaust"))
    return isOn.value
      ? "text-green-600 dark:text-green-300"
      : "text-gray-600 dark:text-gray-400";
  return "text-gray-600 dark:text-gray-400";
});

// Device icon
const deviceIcon = computed(() => {
  const titleLower = props.title.toLowerCase();

  // Heater icon
  if (titleLower.includes("heater")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-6 h-6",
      },
      [
        h("path", {
          d: "M12 2C10.9 2 10 2.9 10 4V6H8C6.9 6 6 6.9 6 8V20C6 21.1 6.9 22 8 22H16C17.1 22 18 21.1 18 20V8C18 6.9 17.1 6 16 6H14V4C14 2.9 13.1 2 12 2M12 4C12.6 4 13 4.4 13 5V6H11V5C11 4.4 11.4 4 12 4M8 8H16V20H8V8M10 10V18H11V10H10M12.5 10V18H13.5V10H12.5Z",
        }),
      ]
    );
  }

  // Fan icon
  if (titleLower.includes("fan")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class:
          isOn.value && isConnected.value ? "w-6 h-6 animate-spin" : "w-6 h-6",
      },
      [
        h("path", {
          d: "M12,11A1,1 0 0,0 11,12A1,1 0 0,0 12,13A1,1 0 0,0 13,12A1,1 0 0,0 12,11M12.5,2C17,2 17.11,5.57 14.75,6.75C13.76,7.24 13.32,8.29 13.13,9.22C13.61,9.42 14.03,9.73 14.35,10.13C18.05,8.13 22.03,8.92 22.03,12.5C22.03,17 18.46,17.1 17.28,14.73C16.78,13.74 15.72,13.3 14.79,13.11C14.59,13.59 14.28,14 13.88,14.34C15.87,18.03 15.08,22 11.5,22C7,22 6.91,18.42 9.27,17.24C10.25,16.75 10.69,15.71 10.89,14.79C10.4,14.59 9.97,14.27 9.65,13.87C5.96,15.85 2,15.07 2,11.5C2,7 5.56,6.89 6.74,9.26C7.24,10.25 8.29,10.68 9.22,10.87C9.41,10.39 9.73,9.97 10.14,9.65C8.15,5.96 8.94,2 12.5,2Z",
        }),
      ]
    );
  }

  // Exhaust icon
  if (titleLower.includes("exhaust")) {
    return h(
      "svg",
      {
        fill: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-6 h-6",
      },
      [
        h("path", {
          d: "M12,2A10,10 0 0,1 22,12A10,10 0 0,1 12,22A10,10 0 0,1 2,12A10,10 0 0,1 12,2M12,4A8,8 0 0,0 4,12A8,8 0 0,0 12,20A8,8 0 0,0 20,12A8,8 0 0,0 12,4M12,6A6,6 0 0,1 18,12A6,6 0 0,1 12,18A6,6 0 0,1 6,12A6,6 0 0,1 12,6M12,8A4,4 0 0,0 8,12A4,4 0 0,0 12,16A4,4 0 0,0 16,12A4,4 0 0,0 12,8M12,10A2,2 0 0,1 14,12A2,2 0 0,1 12,14A2,2 0 0,1 10,12A2,2 0 0,1 12,10Z",
        }),
      ]
    );
  }

  // Default icon
  return h(
    "svg",
    {
      fill: "none",
      stroke: "currentColor",
      viewBox: "0 0 24 24",
      class: "w-6 h-6",
    },
    [
      h("path", {
        "stroke-linecap": "round",
        "stroke-linejoin": "round",
        "stroke-width": "2",
        d: "M13 10V3L4 14h7v7l9-11h-7z",
      }),
    ]
  );
});

// Button icon
const buttonIcon = computed(() => {
  if (isOn.value) {
    // Power off icon
    return h(
      "svg",
      {
        fill: "none",
        stroke: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-5 h-5",
      },
      [
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M18.364 18.364A9 9 0 005.636 5.636m12.728 12.728A9 9 0 015.636 5.636m12.728 12.728L5.636 5.636",
        }),
      ]
    );
  } else {
    // Power on icon
    return h(
      "svg",
      {
        fill: "none",
        stroke: "currentColor",
        viewBox: "0 0 24 24",
        class: "w-5 h-5",
      },
      [
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M13 10V3L4 14h7v7l9-11h-7z",
        }),
      ]
    );
  }
});

const handleToggle = () => {
  if (!isConnected.value) return;
  emit("toggle");
};
</script>
