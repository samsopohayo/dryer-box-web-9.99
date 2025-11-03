// FILE: src/components/TimerControl.vue (UPDATED - With Pause/Resume Button) //
============================================
<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
  >
    <div class="text-center mb-8">
      <h3 class="text-xl font-bold text-gray-800 dark:text-white mb-2">
        Waktu Tersisa
      </h3>
      <div class="relative inline-block">
        <p
          class="text-6xl font-bold mb-4 transition-colors duration-300 font-mono"
          :class="statusColor"
        >
          {{ displayTime }}
        </p>
        <div
          v-if="timerStore.isRunning"
          class="absolute -top-2 -right-2 w-4 h-4 bg-green-500 rounded-full animate-pulse"
        ></div>
        <div
          v-else-if="timerStore.isPaused"
          class="absolute -top-2 -right-2 w-4 h-4 bg-yellow-500 rounded-full"
        ></div>
      </div>
      <p
        v-if="timerStore.isRunning"
        class="text-sm text-blue-600 dark:text-blue-400 font-medium"
      >
        ⏱️ Timer sedang berjalan...
      </p>
      <p
        v-else-if="timerStore.isPaused"
        class="text-sm text-yellow-600 dark:text-yellow-400 font-medium"
      >
        ⏸️ Timer dijeda
      </p>
      <p
        v-else-if="timerStore.timerData.enabled && !timerStore.isRunning"
        class="text-sm text-orange-600 dark:text-orange-400 font-medium"
      >
        ⏳ Menunggu konfirmasi dari sistem...
      </p>
      <p v-else class="text-sm text-gray-500 dark:text-gray-400">
        Timer tidak aktif
      </p>
    </div>

    <div class="mb-6">
      <h4 class="font-bold text-gray-800 dark:text-white mb-4">Atur Timer</h4>
      <div class="grid grid-cols-4 gap-4">
        <!-- Hari Input -->
        <div>
          <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
            >Hari</label
          >
          <div class="relative">
            <input
              v-model.number="days"
              type="number"
              min="0"
              :disabled="timerStore.timerData.enabled"
              class="w-full px-4 py-2 pr-10 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white text-center font-mono text-lg disabled:opacity-50 disabled:cursor-not-allowed"
              @input="handleDaysInput"
              @blur="normalizeTime"
              @keydown="handleKeyDown"
            />
            <div
              class="absolute right-2 top-1/2 -translate-y-1/2 flex flex-col gap-0.5"
            >
              <button
                @click="incrementDays"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M14.707 12.707a1 1 0 01-1.414 0L10 9.414l-3.293 3.293a1 1 0 01-1.414-1.414l4-4a1 1 0 011.414 0l4 4a1 1 0 010 1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
              <button
                @click="decrementDays"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
            </div>
          </div>
        </div>

        <!-- Jam Input -->
        <div>
          <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
            >Jam</label
          >
          <div class="relative">
            <input
              v-model.number="hours"
              type="number"
              min="0"
              :disabled="timerStore.timerData.enabled"
              class="w-full px-4 py-2 pr-10 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white text-center font-mono text-lg disabled:opacity-50 disabled:cursor-not-allowed"
              @input="handleHoursInput"
              @blur="normalizeTime"
              @keydown="handleKeyDown"
            />
            <div
              class="absolute right-2 top-1/2 -translate-y-1/2 flex flex-col gap-0.5"
            >
              <button
                @click="incrementHours"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M14.707 12.707a1 1 0 01-1.414 0L10 9.414l-3.293 3.293a1 1 0 01-1.414-1.414l4-4a1 1 0 011.414 0l4 4a1 1 0 010 1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
              <button
                @click="decrementHours"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
            </div>
          </div>
        </div>

        <!-- Menit Input -->
        <div>
          <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
            >Menit</label
          >
          <div class="relative">
            <input
              v-model.number="minutes"
              type="number"
              min="0"
              :disabled="timerStore.timerData.enabled"
              class="w-full px-4 py-2 pr-10 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white text-center font-mono text-lg disabled:opacity-50 disabled:cursor-not-allowed"
              @input="handleMinutesInput"
              @blur="normalizeTime"
              @keydown="handleKeyDown"
            />
            <div
              class="absolute right-2 top-1/2 -translate-y-1/2 flex flex-col gap-0.5"
            >
              <button
                @click="incrementMinutes"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M14.707 12.707a1 1 0 01-1.414 0L10 9.414l-3.293 3.293a1 1 0 01-1.414-1.414l4-4a1 1 0 011.414 0l4 4a1 1 0 010 1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
              <button
                @click="decrementMinutes"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
            </div>
          </div>
        </div>

        <!-- Detik Input -->
        <div>
          <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
            >Detik</label
          >
          <div class="relative">
            <input
              v-model.number="seconds"
              type="number"
              min="0"
              :disabled="timerStore.timerData.enabled"
              class="w-full px-4 py-2 pr-10 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white text-center font-mono text-lg disabled:opacity-50 disabled:cursor-not-allowed"
              @input="handleSecondsInput"
              @blur="normalizeTime"
              @keydown="handleKeyDown"
            />
            <div
              class="absolute right-2 top-1/2 -translate-y-1/2 flex flex-col gap-0.5"
            >
              <button
                @click="incrementSeconds"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M14.707 12.707a1 1 0 01-1.414 0L10 9.414l-3.293 3.293a1 1 0 01-1.414-1.414l4-4a1 1 0 011.414 0l4 4a1 1 0 010 1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
              <button
                @click="decrementSeconds"
                :disabled="timerStore.timerData.enabled"
                type="button"
                class="p-1 hover:bg-gray-200 dark:hover:bg-gray-600 rounded transition disabled:opacity-50 disabled:cursor-not-allowed"
              >
                <svg
                  class="w-3 h-3 text-gray-600 dark:text-gray-400"
                  fill="currentColor"
                  viewBox="0 0 20 20"
                >
                  <path
                    fill-rule="evenodd"
                    d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z"
                    clip-rule="evenodd"
                  />
                </svg>
              </button>
            </div>
          </div>
        </div>
      </div>

      <!-- Total Duration Display -->
      <div
        v-if="totalSeconds > 0"
        class="mt-4 p-3 bg-gray-50 dark:bg-gray-700 rounded-lg"
      >
        <p class="text-sm text-gray-600 dark:text-gray-400 text-center">
          <strong>Total Durasi:</strong>
          <span class="font-mono text-lg text-gray-800 dark:text-white ml-2">
            {{ formatTotalDuration }}
          </span>
        </p>
      </div>
    </div>

    <div class="flex space-x-4">
      <!-- Start/Pause Button (Transforms based on state) -->
      <button
        @click="handleStartPause"
        :disabled="!isValidDuration && !timerStore.timerData.enabled"
        :class="startPauseButtonClass"
        class="flex-1 py-3 rounded-lg font-semibold transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
      >
        <component :is="startPauseIcon" class="w-5 h-5" />
        <span>{{ startPauseText }}</span>
      </button>

      <!-- Stop Button -->
      <button
        @click="stopTimer"
        :disabled="!timerStore.timerData.enabled"
        class="flex-1 bg-red-500 text-white py-3 rounded-lg font-semibold hover:bg-red-600 transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
      >
        <svg
          class="w-5 h-5"
          fill="none"
          stroke="currentColor"
          viewBox="0 0 24 24"
        >
          <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
          />
          <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M9 10a1 1 0 011-1h4a1 1 0 011 1v4a1 1 0 01-1 1h-4a1 1 0 01-1-1v-4z"
          />
        </svg>
        <span>Stop Timer</span>
      </button>
    </div>

    <div class="mt-4 p-4 bg-blue-50 dark:bg-blue-900 rounded-lg">
      <p class="text-xs text-gray-700 dark:text-gray-300">
        <strong>💡 Catatan:</strong>
      </p>
      <ul
        class="text-xs text-gray-700 dark:text-gray-300 mt-2 ml-4 list-disc space-y-1"
      >
        <li>
          Input otomatis terkonversi: ≥60 detik → menit, ≥60 menit → jam, ≥24
          jam → hari
        </li>
        <li>
          Klik "Mulai Timer" untuk memulai, klik lagi untuk "Pause/Resume"
        </li>
        <li>Timer mulai countdown setelah sistem alat mengkonfirmasi</li>
        <li>Jika timer aktif, target selesai adalah timer (bukan kadar air)</li>
        <li>Timer akan otomatis reset ke 0 saat session baru dimulai</li>
      </ul>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch, h } from "vue";
import { useTimerStore } from "@/stores/timer";

const timerStore = useTimerStore();
const days = ref(0);
const hours = ref(0);
const minutes = ref(0);
const seconds = ref(0);

const displayTime = computed(() => {
  return timerStore.timerDisplay;
});

const statusColor = computed(() => {
  if (timerStore.isRunning) {
    return "text-blue-600 dark:text-blue-400";
  } else if (timerStore.isPaused) {
    return "text-yellow-600 dark:text-yellow-400";
  } else {
    return "text-gray-800 dark:text-white";
  }
});

const totalSeconds = computed(() => {
  return (
    days.value * 86400 + hours.value * 3600 + minutes.value * 60 + seconds.value
  );
});

const formatTotalDuration = computed(() => {
  const total = totalSeconds.value;
  const d = Math.floor(total / 86400);
  const h = Math.floor((total % 86400) / 3600);
  const m = Math.floor((total % 3600) / 60);
  const s = total % 60;

  const parts = [];
  if (d > 0) parts.push(`${d} hari`);
  if (h > 0) parts.push(`${h} jam`);
  if (m > 0) parts.push(`${m} menit`);
  if (s > 0) parts.push(`${s} detik`);

  return parts.length > 0 ? parts.join(", ") : "0 detik";
});

const isValidDuration = computed(() => {
  return totalSeconds.value > 0;
});

// Start/Pause button state
const startPauseText = computed(() => {
  if (timerStore.isPaused) {
    return "Lanjutkan Timer";
  } else if (timerStore.isRunning || timerStore.timerData.enabled) {
    return "Pause Timer";
  } else {
    return "Mulai Timer";
  }
});

const startPauseButtonClass = computed(() => {
  if (timerStore.isPaused) {
    return "bg-blue-500 text-white hover:bg-blue-600";
  } else if (timerStore.isRunning || timerStore.timerData.enabled) {
    return "bg-yellow-500 text-white hover:bg-yellow-600";
  } else {
    return "bg-green-500 text-white hover:bg-green-600";
  }
});

const startPauseIcon = computed(() => {
  if (timerStore.isPaused) {
    // Resume icon (play)
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
          d: "M14.752 11.168l-3.197-2.132A1 1 0 0010 9.87v4.263a1 1 0 001.555.832l3.197-2.132a1 1 0 000-1.664z",
        }),
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M21 12a9 9 0 11-18 0 9 9 0 0118 0z",
        }),
      ]
    );
  } else if (timerStore.isRunning || timerStore.timerData.enabled) {
    // Pause icon
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
          d: "M10 9v6m4-6v6m7-3a9 9 0 11-18 0 9 9 0 0118 0z",
        }),
      ]
    );
  } else {
    // Start icon (play)
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
          d: "M14.752 11.168l-3.197-2.132A1 1 0 0010 9.87v4.263a1 1 0 001.555.832l3.197-2.132a1 1 0 000-1.664z",
        }),
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M21 12a9 9 0 11-18 0 9 9 0 0118 0z",
        }),
      ]
    );
  }
});

// Normalize time - convert overflow automatically
const normalizeTime = () => {
  // Convert seconds to minutes
  if (seconds.value >= 60) {
    const addMinutes = Math.floor(seconds.value / 60);
    minutes.value += addMinutes;
    seconds.value = seconds.value % 60;
  }

  // Convert minutes to hours
  if (minutes.value >= 60) {
    const addHours = Math.floor(minutes.value / 60);
    hours.value += addHours;
    minutes.value = minutes.value % 60;
  }

  // Convert hours to days
  if (hours.value >= 24) {
    const addDays = Math.floor(hours.value / 24);
    days.value += addDays;
    hours.value = hours.value % 24;
  }

  // Prevent negative values
  if (days.value < 0) days.value = 0;
  if (hours.value < 0) hours.value = 0;
  if (minutes.value < 0) minutes.value = 0;
  if (seconds.value < 0) seconds.value = 0;

  // Handle NaN
  if (isNaN(days.value)) days.value = 0;
  if (isNaN(hours.value)) hours.value = 0;
  if (isNaN(minutes.value)) minutes.value = 0;
  if (isNaN(seconds.value)) seconds.value = 0;
};

// Input handlers
const handleDaysInput = () => {
  if (days.value < 0) days.value = 0;
  if (isNaN(days.value)) days.value = 0;
};

const handleHoursInput = () => {
  if (hours.value < 0) hours.value = 0;
  if (isNaN(hours.value)) hours.value = 0;
};

const handleMinutesInput = () => {
  if (minutes.value < 0) minutes.value = 0;
  if (isNaN(minutes.value)) minutes.value = 0;
};

const handleSecondsInput = () => {
  if (seconds.value < 0) seconds.value = 0;
  if (isNaN(seconds.value)) seconds.value = 0;
};

// Increment/Decrement functions
const incrementDays = () => {
  days.value++;
};

const decrementDays = () => {
  if (days.value > 0) days.value--;
};

const incrementHours = () => {
  hours.value++;
  normalizeTime();
};

const decrementHours = () => {
  if (hours.value > 0) {
    hours.value--;
  } else if (days.value > 0) {
    days.value--;
    hours.value = 23;
  }
};

const incrementMinutes = () => {
  minutes.value++;
  normalizeTime();
};

const decrementMinutes = () => {
  if (minutes.value > 0) {
    minutes.value--;
  } else if (hours.value > 0 || days.value > 0) {
    if (hours.value > 0) {
      hours.value--;
    } else if (days.value > 0) {
      days.value--;
      hours.value = 23;
    }
    minutes.value = 59;
  }
};

const incrementSeconds = () => {
  seconds.value++;
  normalizeTime();
};

const decrementSeconds = () => {
  if (seconds.value > 0) {
    seconds.value--;
  } else if (minutes.value > 0 || hours.value > 0 || days.value > 0) {
    if (minutes.value > 0) {
      minutes.value--;
    } else if (hours.value > 0) {
      hours.value--;
      minutes.value = 59;
    } else if (days.value > 0) {
      days.value--;
      hours.value = 23;
      minutes.value = 59;
    }
    seconds.value = 59;
  }
};

// Handle keyboard input - only allow numbers
const handleKeyDown = (event: KeyboardEvent) => {
  const allowedKeys = [
    "Backspace",
    "Delete",
    "ArrowLeft",
    "ArrowRight",
    "Tab",
    "ArrowUp",
    "ArrowDown",
  ];

  if (allowedKeys.includes(event.key)) {
    return;
  }

  if (!/^\d$/.test(event.key)) {
    event.preventDefault();
  }
};

// Main handler for Start/Pause button
const handleStartPause = () => {
  if (timerStore.isPaused) {
    // Resume timer
    timerStore.resumeTimer();
  } else if (timerStore.isRunning || timerStore.timerData.enabled) {
    // Pause timer
    timerStore.pauseTimer();
  } else {
    // Start new timer
    startTimer();
  }
};

const startTimer = async () => {
  if (!isValidDuration.value) return;

  // Normalize one more time before sending
  normalizeTime();

  console.log("🚀 Starting timer:", {
    days: days.value,
    hours: hours.value,
    minutes: minutes.value,
    seconds: seconds.value,
    totalSeconds: totalSeconds.value,
  });

  await timerStore.setTimer(
    days.value * 24 + hours.value,
    minutes.value,
    seconds.value
  );
};

const stopTimer = async () => {
  await timerStore.stopTimer();
  // Reset input fields
  days.value = 0;
  hours.value = 0;
  minutes.value = 0;
  seconds.value = 0;
};

// Watch untuk timer data dari store (untuk reset dari session baru)
watch(
  () => timerStore.timerData,
  (newData) => {
    if (!newData.enabled && newData.duration === 0 && newData.remaining === 0) {
      // Timer direset dari luar (session baru atau manual stop)
      days.value = 0;
      hours.value = 0;
      minutes.value = 0;
      seconds.value = 0;
    }
  },
  { deep: true }
);
</script>

<style scoped>
/* Hide default number input spinners */
input[type="number"]::-webkit-inner-spin-button,
input[type="number"]::-webkit-outer-spin-button {
  -webkit-appearance: none;
  margin: 0;
}

input[type="number"] {
  -moz-appearance: textfield;
}
</style>
