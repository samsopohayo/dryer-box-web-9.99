<template>
  <header
    class="bg-white dark:bg-gray-800 shadow-md fixed top-0 left-0 right-0 transition-all duration-300 z-[40]"
  >
    <div class="px-4 sm:px-6 lg:px-8">
      <!-- Top Row -->
      <div
        class="flex justify-between items-center py-3 border-b border-gray-200 dark:border-gray-700"
      >
        <!-- Left Section: Menu Toggle & Title -->
        <div class="flex items-center space-x-4 flex-1">
          <button
            @click="toggleSidebar"
            class="p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
            title="Toggle menu"
          >
            <svg
              class="w-6 h-6 text-gray-700 dark:text-gray-300"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M4 6h16M4 12h16m-7 6h7"
              />
            </svg>
          </button>

          <h1 class="text-xl font-bold text-gray-800 dark:text-white">
            PENGERING CABAI
          </h1>
        </div>

        <!-- Center Section: Alert Badge -->
        <div class="flex-1 flex justify-center">
          <div
            v-if="showAlert"
            class="flex items-center space-x-2 px-4 py-1.5 rounded-lg"
            :class="alertClass"
          >
            <svg
              class="w-4 h-4"
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
            <span class="text-xs font-semibold">{{ alertText }}</span>
          </div>
        </div>

        <!-- Right Section: Weather, Notification, Dark Mode -->
        <div class="flex items-center space-x-3 flex-1 justify-end">
          <!-- Weather & Location -->
          <div
            class="hidden md:flex items-center space-x-2 text-xs text-gray-600 dark:text-gray-300"
          >
            <component
              :is="weatherIcon"
              class="w-5 h-5"
              :class="weatherIconColor"
            />
            <span>{{ locationStore.weather.temperature }}°C</span>
            <span class="text-gray-400">|</span>
            <span>{{ locationText }}</span>
          </div>

          <!-- Notification Bell -->
          <div class="relative" ref="notificationRef">
            <button
              @click="toggleNotifications"
              class="relative p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
              title="Notifications"
            >
              <svg
                class="w-5 h-5 text-gray-700 dark:text-gray-300"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  stroke-width="2"
                  d="M15 17h5l-1.405-1.405A2.032 2.032 0 0118 14.158V11a6.002 6.002 0 00-4-5.659V5a2 2 0 10-4 0v.341C7.67 6.165 6 8.388 6 11v3.159c0 .538-.214 1.055-.595 1.436L4 17h5m6 0v1a3 3 0 11-6 0v-1m6 0H9"
                />
              </svg>
              <span
                v-if="notificationStore.unreadCount > 0"
                class="absolute top-1 right-1 w-4 h-4 bg-red-500 text-white text-xs rounded-full flex items-center justify-center font-semibold"
              >
                {{
                  notificationStore.unreadCount > 9
                    ? "9+"
                    : notificationStore.unreadCount
                }}
              </span>
            </button>

            <!-- Notification Panel -->
            <transition
              enter-active-class="transition ease-out duration-200"
              enter-from-class="opacity-0 scale-95"
              enter-to-class="opacity-100 scale-100"
              leave-active-class="transition ease-in duration-150"
              leave-from-class="opacity-100 scale-100"
              leave-to-class="opacity-0 scale-95"
            >
              <div
                v-if="showNotifications"
                class="absolute right-0 mt-2 w-96 bg-white dark:bg-gray-800 rounded-lg shadow-xl border border-gray-200 dark:border-gray-700 max-h-96 overflow-y-auto"
              >
                <div
                  class="p-4 border-b border-gray-200 dark:border-gray-700 flex justify-between items-center sticky top-0 bg-white dark:bg-gray-800 z-10"
                >
                  <h3 class="font-bold text-gray-800 dark:text-white">
                    Notifikasi
                  </h3>
                  <div class="flex items-center space-x-2">
                    <button
                      @click="notificationStore.markAllAsRead"
                      class="text-sm text-blue-600 hover:underline"
                    >
                      Tandai semua dibaca
                    </button>
                    <button
                      @click="showNotifications = false"
                      class="p-1 hover:bg-gray-100 dark:hover:bg-gray-700 rounded transition"
                    >
                      <svg
                        class="w-5 h-5 text-gray-500"
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

                <div
                  v-if="notificationStore.notifications.length === 0"
                  class="p-8 text-center text-gray-500"
                >
                  <svg
                    class="w-16 h-16 mx-auto mb-4 text-gray-300"
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      stroke-linecap="round"
                      stroke-linejoin="round"
                      stroke-width="2"
                      d="M20 13V6a2 2 0 00-2-2H6a2 2 0 00-2 2v7m16 0v5a2 2 0 01-2 2H6a2 2 0 01-2-2v-5m16 0h-2.586a1 1 0 00-.707.293l-2.414 2.414a1 1 0 01-.707.293h-3.172a1 1 0 01-.707-.293l-2.414-2.414A1 1 0 006.586 13H4"
                    />
                  </svg>
                  <p class="text-sm">Tidak ada notifikasi</p>
                </div>

                <div
                  v-for="notif in notificationStore.notifications"
                  :key="notif.id"
                  @click="handleNotificationClick(notif.id)"
                  class="p-4 border-b border-gray-200 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-700 cursor-pointer transition"
                  :class="{ 'bg-blue-50 dark:bg-blue-900/20': !notif.read }"
                >
                  <div class="flex items-start space-x-3">
                    <div class="flex-shrink-0 mt-1">
                      <div
                        class="w-2 h-2 rounded-full"
                        :class="{
                          'bg-blue-500': notif.type === 'info',
                          'bg-yellow-500': notif.type === 'warning',
                          'bg-red-500': notif.type === 'error',
                          'bg-green-500': notif.type === 'success',
                        }"
                      ></div>
                    </div>
                    <div class="flex-1 min-w-0">
                      <p
                        class="font-semibold text-gray-800 dark:text-white text-sm"
                      >
                        {{ notif.title }}
                      </p>
                      <p class="text-sm text-gray-600 dark:text-gray-300 mt-1">
                        {{ notif.message }}
                      </p>
                      <p class="text-xs text-gray-400 mt-2">
                        {{ formatTime(notif.timestamp) }}
                      </p>
                    </div>
                    <div v-if="!notif.read" class="flex-shrink-0">
                      <div class="w-2 h-2 bg-blue-500 rounded-full"></div>
                    </div>
                  </div>
                </div>
              </div>
            </transition>
          </div>

          <!-- Dark Mode Toggle -->
          <button
            @click="themeStore.toggleTheme"
            class="p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
            title="Toggle dark mode"
          >
            <svg
              v-if="!themeStore.isDark"
              class="w-5 h-5 text-gray-700 dark:text-gray-300"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M20.354 15.354A9 9 0 018.646 3.646 9.003 9.003 0 0012 21a9.003 9.003 0 008.354-5.646z"
              />
            </svg>
            <svg
              v-else
              class="w-5 h-5 text-yellow-400"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z"
              />
            </svg>
          </button>
        </div>
      </div>

      <!-- Bottom Row - Page Info & Status -->
      <div class="py-2 flex justify-between items-center text-sm">
        <p class="text-gray-600 dark:text-gray-300">
          {{ currentPage }}
        </p>
        <div
          class="flex items-center space-x-4 text-gray-600 dark:text-gray-300"
        >
          <span>{{ statusText }}</span>
          <span class="text-gray-400">•</span>
          <span>{{ currentDateTime }}</span>
        </div>
      </div>
    </div>
  </header>
</template>

<script setup lang="ts">
import { ref, computed, h, onMounted, onBeforeUnmount, watch } from "vue";
import { useRoute } from "vue-router";
import { useDryerStore } from "@/stores/dryer";
import { useTimerStore } from "@/stores/timer";
import { useAuthStore } from "@/stores/auth";
import { useLocationStore } from "@/stores/location";
import { useThemeStore } from "@/stores/theme";
import { useNotificationStore } from "@/stores/notification";

const route = useRoute();
const dryerStore = useDryerStore();
const timerStore = useTimerStore();
const authStore = useAuthStore();
const locationStore = useLocationStore();
const themeStore = useThemeStore();
const notificationStore = useNotificationStore();

const showNotifications = ref(false);
const notificationRef = ref<HTMLElement | null>(null);
const currentDateTime = ref("");

const wasDisconnected = ref(false);
const showReconnectedAlert = ref(false);
let reconnectedTimeout: number | null = null;

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

watch(isConnected, (newStatus, oldStatus) => {
  if (newStatus && !oldStatus && wasDisconnected.value) {
    showReconnectedAlert.value = true;
    wasDisconnected.value = false;

    if (reconnectedTimeout !== null) {
      clearTimeout(reconnectedTimeout);
    }

    reconnectedTimeout = window.setTimeout(() => {
      showReconnectedAlert.value = false;
    }, 10000);
  }

  if (!newStatus && oldStatus) {
    wasDisconnected.value = true;
    showReconnectedAlert.value = false;
  }
});

const emit = defineEmits<{
  toggleSidebar: [];
}>();

const toggleSidebar = () => {
  emit("toggleSidebar");
};

const currentPage = computed(() => {
  const pages: Record<string, string> = {
    Dashboard: "Dashboard",
    Control: "Kontrol dan Timer",
    History: "Rekap Pengeringan",
  };
  return pages[route.name as string] || "";
});

const locationText = computed(() => locationStore.location.city);

// Fungsi untuk mendapatkan offset timezone dari koordinat (dalam jam)
const getTimezoneOffset = (lon: number): number => {
  // Hitung offset berdasarkan longitude
  // Setiap 15 derajat = 1 jam perbedaan
  const baseOffset = Math.round(lon / 15);

  // Fine-tuning untuk zona waktu tertentu yang tidak tepat 15 derajat
  // Asia/Kolkata (India) = UTC+5.5
  if (lon >= 68 && lon <= 97 && lon < 100) {
    return 5.5;
  }

  // Asia/Kathmandu (Nepal) = UTC+5.75
  if (lon >= 80 && lon <= 88 && lon < 90) {
    return 5.75;
  }

  return baseOffset;
};

// Update waktu berdasarkan lokasi dengan offset manual
const updateDateTime = () => {
  const lat = locationStore.location.lat;
  const lon = locationStore.location.lon;

  if (lat !== 0 && lon !== 0) {
    // Dapatkan waktu UTC
    const now = new Date();
    const utcTime = now.getTime() + now.getTimezoneOffset() * 60000;

    // Hitung offset target dalam milidetik
    const targetOffset = getTimezoneOffset(lon);
    const targetTime = new Date(utcTime + targetOffset * 3600000);

    // Format waktu
    const day = targetTime.getDate();
    const monthNames = [
      "Jan",
      "Feb",
      "Mar",
      "Apr",
      "Mei",
      "Jun",
      "Jul",
      "Agu",
      "Sep",
      "Okt",
      "Nov",
      "Des",
    ];
    const month = monthNames[targetTime.getMonth()];
    const year = targetTime.getFullYear();
    const hours = targetTime.getHours().toString().padStart(2, "0");
    const minutes = targetTime.getMinutes().toString().padStart(2, "0");
    const seconds = targetTime.getSeconds().toString().padStart(2, "0");

    // Format: "2 Nov 2025, 14:30:45"
    currentDateTime.value = `${day} ${month} ${year}, ${hours}:${minutes}:${seconds}`;
  } else {
    // Default: gunakan waktu lokal
    const now = new Date();
    const day = now.getDate();
    const monthNames = [
      "Jan",
      "Feb",
      "Mar",
      "Apr",
      "Mei",
      "Jun",
      "Jul",
      "Agu",
      "Sep",
      "Okt",
      "Nov",
      "Des",
    ];
    const month = monthNames[now.getMonth()];
    const year = now.getFullYear();
    const hours = now.getHours().toString().padStart(2, "0");
    const minutes = now.getMinutes().toString().padStart(2, "0");
    const seconds = now.getSeconds().toString().padStart(2, "0");

    currentDateTime.value = `${day} ${month} ${year}, ${hours}:${minutes}:${seconds}`;
  }
};

// Only show alert for REAL errors (not "None" or "No active errors")
const showAlert = computed(() => {
  const errorMsg = dryerStore.systemStatus.last_error;
  const hasRealError =
    errorMsg &&
    errorMsg !== "None" &&
    errorMsg !== "No active errors" &&
    errorMsg.trim() !== "";

  return (
    !isConnected.value ||
    showReconnectedAlert.value ||
    timerStore.isRunning ||
    dryerStore.statusData.door_open ||
    dryerStore.statusData.temp_protection ||
    hasRealError
  );
});

const alertClass = computed(() => {
  if (!isConnected.value) return "bg-red-600 text-white";
  if (showReconnectedAlert.value) return "bg-green-600 text-white";
  if (dryerStore.statusData.temp_protection) return "bg-red-500 text-white";
  if (dryerStore.statusData.door_open) return "bg-yellow-500 text-white";

  const errorMsg = dryerStore.systemStatus.last_error;
  if (
    errorMsg &&
    errorMsg !== "None" &&
    errorMsg !== "No active errors" &&
    errorMsg.trim() !== ""
  ) {
    return "bg-red-500 text-white";
  }

  return "bg-blue-600 text-white";
});

const alertText = computed(() => {
  if (!isConnected.value) return "SISTEM TIDAK TERKONEKSI";
  if (showReconnectedAlert.value) return "SISTEM SUDAH TERKONEKSI KEMBALI";
  if (dryerStore.statusData.temp_protection) return "PROTEKSI SUHU AKTIF";
  if (dryerStore.statusData.door_open) return "PINTU TERBUKA";

  const errorMsg = dryerStore.systemStatus.last_error;
  if (
    errorMsg &&
    errorMsg !== "None" &&
    errorMsg !== "No active errors" &&
    errorMsg.trim() !== ""
  ) {
    return "ERROR: " + errorMsg;
  }

  if (timerStore.isRunning) return "Timer: " + timerStore.timerDisplay;
  return "";
});

const statusText = computed(() => {
  return `Status: ${dryerStore.statusData.pengeringan}`;
});

// Weather icon component
const weatherIcon = computed(() => {
  const condition = locationStore.weather.condition;

  if (condition === "cerah") {
    return h(
      "svg",
      {
        class: "w-5 h-5",
        fill: "none",
        stroke: "currentColor",
        viewBox: "0 0 24 24",
      },
      [
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M12 3v1m0 16v1m9-9h-1M4 12H3m15.364 6.364l-.707-.707M6.343 6.343l-.707-.707m12.728 0l-.707.707M6.343 17.657l-.707.707M16 12a4 4 0 11-8 0 4 4 0 018 0z",
        }),
      ]
    );
  } else if (condition === "berawan") {
    return h(
      "svg",
      {
        class: "w-5 h-5",
        fill: "none",
        stroke: "currentColor",
        viewBox: "0 0 24 24",
      },
      [
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M3 15a4 4 0 004 4h9a5 5 0 10-.1-9.999 5.002 5.002 0 10-9.78 2.096A4.001 4.001 0 003 15z",
        }),
      ]
    );
  } else {
    return h(
      "svg",
      {
        class: "w-5 h-5",
        fill: "none",
        stroke: "currentColor",
        viewBox: "0 0 24 24",
      },
      [
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M3 15a4 4 0 004 4h9a5 5 0 10-.1-9.999 5.002 5.002 0 10-9.78 2.096A4.001 4.001 0 003 15z",
        }),
        h("path", {
          "stroke-linecap": "round",
          "stroke-linejoin": "round",
          "stroke-width": "2",
          d: "M12 19l-2 4m4-4l2 4m-6-4l-2 4",
        }),
      ]
    );
  }
});

const weatherIconColor = computed(() => {
  const condition = locationStore.weather.condition;
  if (condition === "cerah") return "text-yellow-500";
  if (condition === "berawan") return "text-gray-400";
  return "text-blue-500";
});

const toggleNotifications = () => {
  showNotifications.value = !showNotifications.value;
};

const handleNotificationClick = (id: string) => {
  notificationStore.markAsRead(id);
};

const formatTime = (timestamp: number) => {
  const now = Date.now();
  const diff = now - timestamp;
  const minutes = Math.floor(diff / 60000);

  if (minutes < 1) return "Baru saja";
  if (minutes < 60) return `${minutes} menit yang lalu`;

  const hours = Math.floor(minutes / 60);
  if (hours < 24) return `${hours} jam yang lalu`;

  const days = Math.floor(hours / 24);
  return `${days} hari yang lalu`;
};

// Click outside handler
const handleClickOutside = (event: MouseEvent) => {
  if (
    showNotifications.value &&
    notificationRef.value &&
    !notificationRef.value.contains(event.target as Node)
  ) {
    showNotifications.value = false;
  }
};

let dateTimeInterval: number | null = null;

// Watch untuk perubahan lokasi
watch(
  () => [locationStore.location.lat, locationStore.location.lon],
  () => {
    updateDateTime();
  }
);

onMounted(() => {
  document.addEventListener("click", handleClickOutside);

  wasDisconnected.value = !isConnected.value;

  // Update datetime immediately
  updateDateTime();

  // Update datetime setiap detik
  dateTimeInterval = window.setInterval(updateDateTime, 1000);
});

onBeforeUnmount(() => {
  document.removeEventListener("click", handleClickOutside);

  // Clear interval
  if (dateTimeInterval !== null) {
    clearInterval(dateTimeInterval);
  }

  if (reconnectedTimeout !== null) {
    clearTimeout(reconnectedTimeout);
  }
});
</script>
