<template>
  <header
    class="bg-white dark:bg-gray-800 shadow-lg sticky top-0 z-50 transition-colors"
  >
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
      <div class="flex justify-between items-center py-4">
        <div class="flex items-center space-x-8">
          <h1 class="text-2xl font-bold text-gray-800 dark:text-white">
            PENGERING CABAI
          </h1>
          <p class="text-sm text-gray-600 dark:text-gray-300 hidden md:block">
            {{ currentPage }}
          </p>
        </div>

        <div class="flex items-center space-x-4">
          <div
            v-if="showAlert"
            class="flex items-center space-x-2 px-4 py-2 rounded-lg"
            :class="alertClass"
          >
            <span class="text-sm font-semibold">{{ alertText }}</span>
          </div>

          <div class="text-sm text-gray-600 dark:text-gray-300">
            <p class="font-medium">{{ statusText }}</p>
            <div class="flex items-center space-x-2 text-xs">
              <span>{{ locationText }}</span>
              <span>{{ currentDate }}</span>
              <component
                :is="weatherIcon"
                class="w-5 h-5"
                :class="weatherIconColor"
              />
            </div>
          </div>

          <!-- Notification Bell -->
          <button
            @click="toggleNotifications"
            class="relative p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
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
                d="M15 17h5l-1.405-1.405A2.032 2.032 0 0118 14.158V11a6.002 6.002 0 00-4-5.659V5a2 2 0 10-4 0v.341C7.67 6.165 6 8.388 6 11v3.159c0 .538-.214 1.055-.595 1.436L4 17h5m6 0v1a3 3 0 11-6 0v-1m6 0H9"
              />
            </svg>
            <span
              v-if="notificationStore.unreadCount > 0"
              class="absolute top-1 right-1 w-4 h-4 bg-red-500 text-white text-xs rounded-full flex items-center justify-center"
            >
              {{ notificationStore.unreadCount }}
            </span>
          </button>

          <!-- Dark Mode Toggle -->
          <button
            @click="themeStore.toggleTheme"
            class="p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
          >
            <svg
              v-if="!themeStore.isDark"
              class="w-6 h-6"
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
              class="w-6 h-6 text-yellow-400"
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

          <button
            @click="showMenu = !showMenu"
            class="p-2 hover:bg-gray-100 dark:hover:bg-gray-700 rounded-lg transition"
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
        </div>
      </div>
    </div>

    <!-- Menu Dropdown -->
    <div
      v-if="showMenu"
      class="absolute right-4 mt-2 w-48 bg-white dark:bg-gray-800 rounded-lg shadow-xl border border-gray-200 dark:border-gray-700"
    >
      <button
        @click="logout"
        class="w-full text-left px-4 py-2 hover:bg-gray-100 dark:hover:bg-gray-700 text-red-600 font-semibold"
      >
        Keluar
      </button>
    </div>

    <!-- Notification Panel -->
    <div
      v-if="showNotifications"
      class="absolute right-4 mt-2 w-96 bg-white dark:bg-gray-800 rounded-lg shadow-xl border border-gray-200 dark:border-gray-700 max-h-96 overflow-y-auto"
    >
      <div
        class="p-4 border-b border-gray-200 dark:border-gray-700 flex justify-between items-center"
      >
        <h3 class="font-bold text-gray-800 dark:text-white">Notifikasi</h3>
        <button
          @click="notificationStore.markAllAsRead"
          class="text-sm text-blue-600 hover:underline"
        >
          Tandai semua dibaca
        </button>
      </div>

      <div
        v-if="notificationStore.notifications.length === 0"
        class="p-8 text-center text-gray-500"
      >
        Tidak ada notifikasi
      </div>

      <div
        v-for="notif in notificationStore.notifications"
        :key="notif.id"
        @click="notificationStore.markAsRead(notif.id)"
        class="p-4 border-b border-gray-200 dark:border-gray-700 hover:bg-gray-50 dark:hover:bg-gray-700 cursor-pointer"
        :class="{ 'bg-blue-50 dark:bg-blue-900': !notif.read }"
      >
        <div class="flex items-start space-x-3">
          <div class="flex-shrink-0">
            <div
              class="w-2 h-2 mt-2 rounded-full"
              :class="{
                'bg-blue-500': notif.type === 'info',
                'bg-yellow-500': notif.type === 'warning',
                'bg-red-500': notif.type === 'error',
                'bg-green-500': notif.type === 'success',
              }"
            ></div>
          </div>
          <div class="flex-1">
            <p class="font-semibold text-gray-800 dark:text-white">
              {{ notif.title }}
            </p>
            <p class="text-sm text-gray-600 dark:text-gray-300">
              {{ notif.message }}
            </p>
            <p class="text-xs text-gray-400 mt-1">
              {{ formatTime(notif.timestamp) }}
            </p>
          </div>
        </div>
      </div>
    </div>
  </header>
</template>

<script setup lang="ts">
import { ref, computed, h } from "vue";
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

const showMenu = ref(false);
const showNotifications = ref(false);

const currentPage = computed(() => {
  const pages: Record<string, string> = {
    Dashboard: "Dashboard",
    Control: "Kontrol dan Timer",
    History: "Rekap Pengeringan",
  };
  return pages[route.name as string] || "";
});

const locationText = computed(() => locationStore.location.city);

const showAlert = computed(() => {
  return (
    timerStore.timerData.enabled ||
    dryerStore.statusData.door_open ||
    dryerStore.statusData.temp_protection ||
    dryerStore.systemStatus.last_error !== "None"
  );
});

const alertClass = computed(() => {
  if (dryerStore.statusData.temp_protection) return "bg-red-500 text-white";
  if (dryerStore.statusData.door_open) return "bg-yellow-500 text-white";
  if (dryerStore.systemStatus.last_error !== "None")
    return "bg-red-500 text-white";
  return "bg-red-600 text-white";
});

const alertText = computed(() => {
  if (dryerStore.statusData.temp_protection) return "PROTEKSI SUHU AKTIF";
  if (dryerStore.statusData.door_open) return "PINTU TERBUKA";
  if (dryerStore.systemStatus.last_error !== "None")
    return "ERROR: " + dryerStore.systemStatus.last_error;
  if (timerStore.timerData.enabled) return "Timer: " + timerStore.timerDisplay;
  return "";
});

const statusText = computed(() => {
  return `Status pengeringan: ${dryerStore.statusData.pengeringan}`;
});

const currentDate = computed(() => {
  const now = new Date();
  return now.toLocaleDateString("id-ID", {
    day: "numeric",
    month: "long",
    year: "numeric",
    hour: "2-digit",
    minute: "2-digit",
  });
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
  showMenu.value = false;
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

const logout = () => {
  authStore.logout();
  showMenu.value = false;
};
</script>
