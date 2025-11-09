<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
  >
    <h3 class="font-bold text-lg mb-4 text-gray-800 dark:text-white">
      Pengaturan Lokasi & Cuaca
    </h3>

    <!-- Connection Warning -->
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
        Sistem offline. Data cuaca tidak dapat diperbarui.
      </p>
    </div>

    <div class="mb-4">
      <p class="text-sm text-gray-600 dark:text-gray-400 mb-2">
        Lokasi saat ini:
        <span class="font-semibold">{{ locationStore.location.city }}</span>
      </p>
      <p class="text-xs text-gray-500 dark:text-gray-500">
        Koordinat: {{ locationStore.location.lat.toFixed(4) }},
        {{ locationStore.location.lon.toFixed(4) }}
      </p>
    </div>

    <!-- Weather Card -->
    <div class="mb-4">
      <div class="p-4 rounded-lg" :class="weatherCardClass">
        <div class="flex items-center justify-between">
          <div class="flex items-center space-x-3">
            <component
              :is="weatherIcon"
              class="w-12 h-12"
              :class="weatherIconColor"
            />
            <div>
              <p class="text-sm text-gray-600 dark:text-gray-400">
                Kondisi Cuaca
              </p>
              <p class="text-2xl font-bold text-gray-800 dark:text-white">
                {{ weatherDisplayText }}
              </p>
            </div>
          </div>
          <div class="text-right">
            <p class="text-4xl font-bold text-gray-800 dark:text-white">
              {{ locationStore.weather.temperature }}°C
            </p>
            <p class="text-sm text-gray-600 dark:text-gray-400">
              {{ locationStore.weather.humidity }}% RH
            </p>
          </div>
        </div>

        <!-- Collector Fan Status -->
        <div
          v-if="isConnected && showCollectorInfo"
          class="mt-4 pt-4 border-t"
          :class="collectorBorderClass"
        >
          <div class="flex items-center justify-between">
            <div class="flex items-center space-x-2">
              <svg
                class="w-5 h-5"
                :class="[
                  collectorActive
                    ? 'animate-spin text-blue-600 dark:text-blue-400'
                    : 'text-gray-400',
                ]"
                fill="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  d="M12,11A1,1 0 0,0 11,12A1,1 0 0,0 12,13A1,1 0 0,0 13,12A1,1 0 0,0 12,11M12.5,2C17,2 17.11,5.57 14.75,6.75C13.76,7.24 13.32,8.29 13.13,9.22C13.61,9.42 14.03,9.73 14.35,10.13C18.05,8.13 22.03,8.92 22.03,12.5C22.03,17 18.46,17.1 17.28,14.73C16.78,13.74 15.72,13.3 14.79,13.11C14.59,13.59 14.28,14 13.88,14.34C15.87,18.03 15.08,22 11.5,22C7,22 6.91,18.42 9.27,17.24C10.25,16.75 10.69,15.71 10.89,14.79C10.4,14.59 9.97,14.27 9.65,13.87C5.96,15.85 2,15.07 2,11.5C2,7 5.56,6.89 6.74,9.26C7.24,10.25 8.29,10.68 9.22,10.87C9.41,10.39 9.73,9.97 10.14,9.65C8.15,5.96 8.94,2 12.5,2Z"
                />
              </svg>
              <p class="text-sm font-medium text-gray-700 dark:text-gray-300">
                <strong>Fan Collector:</strong>
              </p>
            </div>
            <span
              class="px-3 py-1 rounded-full text-xs font-bold"
              :class="collectorStatusClass"
            >
              {{ collectorStatusText }}
            </span>
          </div>
          <p class="text-xs text-gray-600 dark:text-gray-400 mt-2">
            {{ collectorReasonText }}
          </p>
        </div>
      </div>
    </div>

    <form @submit.prevent="handleUpdateLocation" class="space-y-4">
      <div>
        <label
          class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2"
        >
          Masukkan Nama Kota atau Daerah
        </label>
        <input
          v-model="cityInput"
          type="text"
          placeholder="Contoh: Yogyakarta, Jakarta, Bali"
          required
          :disabled="locationStore.isLoading"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white disabled:opacity-50 disabled:cursor-not-allowed"
        />
        <p class="text-xs text-gray-500 dark:text-gray-400 mt-1">
          💡 Sistem akan mengkonversi nama daerah ke koordinat secara otomatis
        </p>
      </div>

      <div
        v-if="locationStore.error"
        class="bg-red-50 dark:bg-red-900 border border-red-200 dark:border-red-700 text-red-700 dark:text-red-200 px-4 py-3 rounded-lg"
      >
        {{ locationStore.error }}
      </div>

      <button
        type="submit"
        :disabled="locationStore.isLoading"
        class="w-full bg-primary text-white py-3 rounded-lg font-semibold hover:bg-red-700 transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
      >
        <svg
          v-if="locationStore.isLoading"
          class="animate-spin h-5 w-5 text-white"
          xmlns="http://www.w3.org/2000/svg"
          fill="none"
          viewBox="0 0 24 24"
        >
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
        <span>{{
          locationStore.isLoading ? "Memproses..." : "Update Lokasi"
        }}</span>
      </button>
    </form>

    <div class="mt-4 p-4 bg-blue-50 dark:bg-blue-900 rounded-lg">
      <p class="text-xs text-gray-600 dark:text-gray-300">
        <strong>📌 Info Penting:</strong>
      </p>
      <ul
        class="text-xs text-gray-600 dark:text-gray-300 mt-2 ml-4 list-disc space-y-1"
      >
        <li>
          Data cuaca diambil dari Open-Meteo API dan dikelompokkan menjadi 3
          kondisi: <strong>Cerah</strong>, <strong>Berawan</strong>, dan
          <strong>Hujan</strong>
        </li>
        <li>
          <strong>Fan Collector</strong> akan aktif otomatis saat cuaca
          cerah/berawan dan jam operasi 06:00-17:00
        </li>
        <li>
          Perubahan lokasi akan memperbarui kondisi cuaca secara real-time dan
          memengaruhi kontrol Fan Collector
        </li>
      </ul>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, h } from "vue";
import { useLocationStore } from "@/stores/location";
import { useDryerStore } from "@/stores/dryer";

const locationStore = useLocationStore();
const dryerStore = useDryerStore();
const cityInput = ref("");

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

const weatherDisplayText = computed(() => {
  const desc = locationStore.weather.description;
  return desc.charAt(0).toUpperCase() + desc.slice(1);
});

const weatherCardClass = computed(() => {
  const condition = locationStore.weather.condition;
  if (condition === "cerah") {
    return "bg-gradient-to-br from-yellow-50 to-orange-50 dark:from-yellow-900 dark:to-orange-900";
  } else if (condition === "berawan") {
    return "bg-gradient-to-br from-gray-50 to-blue-50 dark:from-gray-800 dark:to-blue-900";
  } else {
    return "bg-gradient-to-br from-blue-50 to-indigo-50 dark:from-blue-900 dark:to-indigo-900";
  }
});

const weatherIcon = computed(() => {
  const condition = locationStore.weather.condition;

  if (condition === "cerah") {
    return h(
      "svg",
      {
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
          d: "M8 19l-1 3m4-3l-1 3m4-3l-1 3m4-3l-1 3",
        }),
      ]
    );
  }
});

const weatherIconColor = computed(() => {
  const condition = locationStore.weather.condition;
  if (condition === "cerah") return "text-yellow-500 dark:text-yellow-400";
  if (condition === "berawan") return "text-gray-500 dark:text-gray-400";
  return "text-blue-500 dark:text-blue-400";
});

// Show collector info only when system is running
const showCollectorInfo = computed(() => {
  return dryerStore.statusData.pengeringan === "Berjalan";
});

const collectorActive = computed(() => {
  const condition = locationStore.weather.condition;
  const isSunny = condition === "cerah" || condition === "berawan";

  const now = new Date();
  const hour = now.getHours();
  const isCollectorTime = hour >= 6 && hour < 17;

  return isSunny && isCollectorTime;
});

const collectorStatusText = computed(() => {
  return collectorActive.value ? "AKTIF" : "MATI";
});

const collectorStatusClass = computed(() => {
  return collectorActive.value
    ? "bg-green-500 text-white"
    : "bg-gray-400 text-white";
});

const collectorReasonText = computed(() => {
  const condition = locationStore.weather.condition;
  const now = new Date();
  const hour = now.getHours();
  const isCollectorTime = hour >= 6 && hour < 17;

  if (collectorActive.value) {
    return "Kondisi ideal untuk operasi fan collector (cuaca cerah & jam 06:00-17:00)";
  }

  if (!isCollectorTime) {
    return `Fan collector mati karena di luar jam operasi (sekarang: ${hour
      .toString()
      .padStart(2, "0")}:00)`;
  }

  return `Fan collector mati karena cuaca ${condition} (tidak cerah/berawan)`;
});

const collectorBorderClass = computed(() => {
  return collectorActive.value
    ? "border-green-200 dark:border-green-700"
    : "border-gray-200 dark:border-gray-700";
});

const handleUpdateLocation = async () => {
  if (!cityInput.value.trim()) return;

  const success = await locationStore.updateLocation(cityInput.value.trim());

  if (success) {
    cityInput.value = "";
  }
};
</script>
