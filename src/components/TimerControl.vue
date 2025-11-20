<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
  >
    <!-- Connection Status Warning -->
    <div
      v-if="!isConnected"
      class="mb-4 bg-yellow-50 dark:bg-yellow-900 border border-yellow-200 dark:border-yellow-700 rounded-lg p-3 flex items-center space-x-2"
    >
      <svg
        class="w-5 h-5 text-yellow-600 dark:text-yellow-400"
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
      <p class="text-sm text-yellow-800 dark:text-yellow-200">
        Sistem offline. Timer tidak dapat digunakan.
      </p>
    </div>

    <!-- System Not Running Warning -->
    <div
      v-if="!isSystemRunning && isConnected"
      class="mb-4 bg-orange-50 dark:bg-orange-900 border border-orange-200 dark:border-orange-700 rounded-lg p-3 flex items-center space-x-2"
    >
      <svg
        class="w-5 h-5 text-orange-600 dark:text-orange-400"
        fill="none"
        stroke="currentColor"
        viewBox="0 0 24 24"
      >
        <path
          stroke-linecap="round"
          stroke-linejoin="round"
          stroke-width="2"
          d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
        />
      </svg>
      <p class="text-sm text-orange-800 dark:text-orange-200">
        Sistem belum berjalan. Timer hanya bekerja saat proses pengeringan
        aktif.
      </p>
    </div>

    <!-- Timer Completion Message -->
    <div
      v-if="timerStore.showCompletionMessage"
      class="mb-4 bg-green-50 dark:bg-green-900 border border-green-200 dark:border-green-700 rounded-lg p-4"
    >
      <div class="flex items-start justify-between">
        <div class="flex items-center space-x-3">
          <svg
            class="w-6 h-6 text-green-600 dark:text-green-400"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z"
            />
          </svg>
          <div>
            <p class="font-semibold text-green-800 dark:text-green-200">
              ✅ Timer Selesai!
            </p>
            <p class="text-sm text-green-700 dark:text-green-300 mt-1">
              Timer telah selesai secara natural. Durasi telah tercapai.
            </p>
          </div>
        </div>
        <button
          @click="timerStore.dismissMessages"
          class="text-green-600 dark:text-green-400 hover:text-green-800 dark:hover:text-green-200"
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
              d="M6 18L18 6M6 6l12 12"
            />
          </svg>
        </button>
      </div>
    </div>

    <!-- Timer Stopped Message -->
    <div
      v-if="timerStore.showStopMessage"
      class="mb-4 bg-red-50 dark:bg-red-900 border border-red-200 dark:border-red-700 rounded-lg p-4"
    >
      <div class="flex items-start justify-between">
        <div class="flex items-center space-x-3">
          <svg
            class="w-6 h-6 text-red-600 dark:text-red-400"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M10 14l2-2m0 0l2-2m-2 2l-2-2m2 2l2 2m7-2a9 9 0 11-18 0 9 9 0 0118 0z"
            />
          </svg>
          <div>
            <p class="font-semibold text-red-800 dark:text-red-200">
              🛑 Timer Dihentikan
            </p>
            <p class="text-sm text-red-700 dark:text-red-300 mt-1">
              Timer dihentikan secara manual sebelum selesai.
            </p>
          </div>
        </div>
        <button
          @click="timerStore.dismissMessages"
          class="text-red-600 dark:text-red-400 hover:text-red-800 dark:hover:text-red-200"
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
              d="M6 18L18 6M6 6l12 12"
            />
          </svg>
        </button>
      </div>
    </div>

    <div class="text-center mb-8">
      <h3 class="text-xl font-bold text-gray-800 dark:text-white mb-2">
        Timer Kontrol Manual
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

      <!-- Timer Status Text with ESP32 sync indicator -->
      <div class="mt-2">
        <p
          v-if="timerStore.isRunning"
          class="text-sm text-green-600 dark:text-green-400 font-medium flex items-center justify-center space-x-2"
        >
          <span class="relative flex h-3 w-3">
            <span
              class="animate-ping absolute inline-flex h-full w-full rounded-full bg-green-400 opacity-75"
            ></span>
            <span
              class="relative inline-flex rounded-full h-3 w-3 bg-green-500"
            ></span>
          </span>
          <span>Timer berjalan di ESP32...</span>
        </p>
        <p
          v-else-if="timerStore.isPaused"
          class="text-sm text-yellow-600 dark:text-yellow-400 font-medium"
        >
          ⏸️ Timer dijeda
        </p>
        <p
          v-else-if="timerStore.timerData.enabled && !timerStore.isRunning"
          class="text-sm text-orange-600 dark:text-orange-400 font-medium flex items-center justify-center space-x-2"
        >
          <svg class="animate-spin h-4 w-4" fill="none" viewBox="0 0 24 24">
            <circle
              class="opacity-25"
              cx="12"
              cy="12"
              r="10"
              stroke="currentColor"
              stroke-width="4"
            ></circle>
            <path
              class="opacity-75"
              fill="currentColor"
              d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"
            ></path>
          </svg>
          <span>Menunggu ESP32...</span>
        </p>
        <p v-else class="text-sm text-gray-500 dark:text-gray-400">
          Timer tidak aktif
        </p>
      </div>

      <!-- Last sync time -->
      <p
        v-if="lastSyncTime"
        class="text-xs text-gray-400 dark:text-gray-500 mt-1"
      >
        Last update: {{ lastSyncTime }}
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
              max="30"
              :disabled="
                timerStore.timerData.enabled || !isConnected || !isSystemRunning
              "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
              max="23"
              :disabled="
                timerStore.timerData.enabled || !isConnected || !isSystemRunning
              "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
              max="59"
              :disabled="
                timerStore.timerData.enabled || !isConnected || !isSystemRunning
              "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
              max="59"
              :disabled="
                timerStore.timerData.enabled || !isConnected || !isSystemRunning
              "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
                :disabled="
                  timerStore.timerData.enabled ||
                  !isConnected ||
                  !isSystemRunning
                "
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
      <!-- Start/Pause Button -->
      <button
        @click="handleStartPause"
        :disabled="
          (!isValidDuration && !timerStore.timerData.enabled) ||
          !isConnected ||
          !isSystemRunning
        "
        :class="startPauseButtonClass"
        class="flex-1 py-3 rounded-lg font-semibold transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
      >
        <component :is="startPauseIcon" class="w-5 h-5" />
        <span>{{ startPauseText }}</span>
      </button>

      <!-- Stop Button -->
      <button
        @click="stopTimer"
        :disabled="!timerStore.timerData.enabled || !isConnected"
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
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch, h } from "vue";
import { useTimerStore } from "@/stores/timer";
import { useDryerStore } from "@/stores/dryer";

const timerStore = useTimerStore();
const dryerStore = useDryerStore();

const days = ref(0);
const hours = ref(0);
const minutes = ref(0);
const seconds = ref(0);

const lastSyncTime = ref("");

// Check connection status
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

// Check if system is running
const isSystemRunning = computed(() => {
  const processStatus = dryerStore.systemStatus.process_status;
  const status = dryerStore.systemStatus.status;
  return processStatus === "running" || status === "running";
});

const displayTime = computed(() => {
  return timerStore.timerDisplay;
});

const statusColor = computed(() => {
  if (!isConnected.value) {
    return "text-gray-400 dark:text-gray-600";
  }
  if (timerStore.isRunning) {
    return "text-green-600 dark:text-green-400";
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
  if (!isConnected.value || !isSystemRunning.value) {
    return "bg-gray-400 text-white";
  }
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

const normalizeTime = () => {
  if (seconds.value >= 60) {
    const addMinutes = Math.floor(seconds.value / 60);
    minutes.value += addMinutes;
    seconds.value = seconds.value % 60;
  }

  if (minutes.value >= 60) {
    const addHours = Math.floor(minutes.value / 60);
    hours.value += addHours;
    minutes.value = minutes.value % 60;
  }

  if (hours.value >= 24) {
    const addDays = Math.floor(hours.value / 24);
    days.value += addDays;
    hours.value = hours.value % 24;
  }

  if (days.value < 0) days.value = 0;
  if (hours.value < 0) hours.value = 0;
  if (minutes.value < 0) minutes.value = 0;
  if (seconds.value < 0) seconds.value = 0;

  if (isNaN(days.value)) days.value = 0;
  if (isNaN(hours.value)) hours.value = 0;
  if (isNaN(minutes.value)) minutes.value = 0;
  if (isNaN(seconds.value)) seconds.value = 0;
};

const handleDaysInput = () => {
  if (days.value < 0) days.value = 0;
  if (days.value > 30) days.value = 30;
  if (isNaN(days.value)) days.value = 0;
};

const handleHoursInput = () => {
  if (hours.value < 0) hours.value = 0;
  if (hours.value > 23) hours.value = 23;
  if (isNaN(hours.value)) hours.value = 0;
};

const handleMinutesInput = () => {
  if (minutes.value < 0) minutes.value = 0;
  if (minutes.value > 59) minutes.value = 59;
  if (isNaN(minutes.value)) minutes.value = 0;
};

const handleSecondsInput = () => {
  if (seconds.value < 0) seconds.value = 0;
  if (seconds.value > 59) seconds.value = 59;
  if (isNaN(seconds.value)) seconds.value = 0;
};

const incrementDays = () => {
  if (days.value < 30) days.value++;
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

const handleStartPause = async () => {
  if (!isConnected.value || !isSystemRunning.value) {
    console.warn(
      "Cannot start/pause timer: System not connected or not running"
    );
    return;
  }

  if (timerStore.isPaused) {
    console.log("▶️ Resuming timer...");
    await timerStore.resumeTimer();
  } else if (timerStore.isRunning || timerStore.timerData.enabled) {
    console.log("⏸️ Pausing timer...");
    await timerStore.pauseTimer();
  } else {
    await startTimer();
  }
};

const startTimer = async () => {
  if (!isValidDuration.value || !isConnected.value || !isSystemRunning.value) {
    console.warn("Cannot start timer: Invalid duration or system not ready");
    return;
  }

  normalizeTime();

  const totalHours = days.value * 24 + hours.value;

  console.log("🚀 Starting timer:", {
    days: days.value,
    hours: hours.value,
    minutes: minutes.value,
    seconds: seconds.value,
    totalSeconds: totalSeconds.value,
    totalHours: totalHours,
  });

  await timerStore.setTimer(totalHours, minutes.value, seconds.value);

  // Update last sync time
  lastSyncTime.value = new Date().toLocaleTimeString("id-ID");
};

const stopTimer = async () => {
  console.log("🛑 Stopping timer from UI...");
  await timerStore.stopTimer();

  // Reset input fields
  days.value = 0;
  hours.value = 0;
  minutes.value = 0;
  seconds.value = 0;

  // Update last sync time
  lastSyncTime.value = new Date().toLocaleTimeString("id-ID");
};

// Watch timer data changes to update last sync time
watch(
  () => timerStore.timerData,
  (newData, oldData) => {
    // Reset input fields when timer is disabled externally
    if (!newData.enabled && newData.duration === 0 && newData.remaining === 0) {
      days.value = 0;
      hours.value = 0;
      minutes.value = 0;
      seconds.value = 0;
    }

    // Update last sync time when timer data changes
    if (newData.enabled && oldData && newData.remaining !== oldData.remaining) {
      lastSyncTime.value = new Date().toLocaleTimeString("id-ID");
    }
  },
  { deep: true }
);

// Watch for system running status
watch(isSystemRunning, (running) => {
  if (!running && timerStore.timerData.enabled) {
    console.warn("⚠️ System stopped while timer was running");
  }
});
</script>

<style scoped>
input[type="number"]::-webkit-inner-spin-button,
input[type="number"]::-webkit-outer-spin-button {
  -webkit-appearance: none;
  margin: 0;
}

input[type="number"] {
  -moz-appearance: textfield;
}
</style>
