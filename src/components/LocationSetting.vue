// FILE: src/components/LocationSetting.vue (NEW - Location Setting Component)
// ============================================
<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
  >
    <h3 class="font-bold text-lg mb-4 text-gray-800 dark:text-white">
      Atur Lokasi atau Daerah
    </h3>

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

    <div class="mb-4">
      <div class="p-4 bg-gray-50 dark:bg-gray-700 rounded-lg">
        <div class="flex items-center justify-between">
          <div>
            <p class="text-sm text-gray-600 dark:text-gray-400">
              Kondisi Cuaca
            </p>
            <p class="text-2xl font-bold text-gray-800 dark:text-white">
              {{ locationStore.weather.description }}
            </p>
          </div>
          <div class="text-right">
            <p class="text-3xl font-bold text-gray-800 dark:text-white">
              {{ locationStore.weather.temperature }}°C
            </p>
            <p class="text-sm text-gray-600 dark:text-gray-400">
              {{ locationStore.weather.humidity }}% RH
            </p>
          </div>
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
          placeholder="Nama kota atau daerah"
          required
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
        />
        <p class="text-xs text-gray-500 dark:text-gray-400 mt-1">
          Sistem akan mengkonversi nama daerah ke koordinat secara otomatis
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
        class="w-full bg-primary text-white py-3 rounded-lg font-semibold hover:bg-red-700 transition disabled:opacity-50 disabled:cursor-not-allowed"
      >
        {{ locationStore.isLoading ? "Memproses..." : "Update Lokasi" }}
      </button>
    </form>

    <div class="mt-4 p-4 bg-blue-50 dark:bg-blue-900 rounded-lg">
      <p class="text-xs text-gray-600 dark:text-gray-300">
        <strong>Catatan:</strong> Data cuaca diambil dari Open-Meteo API dan
        dikelompokkan menjadi 3 kondisi:
      </p>
      <ul class="text-xs text-gray-600 dark:text-gray-300 mt-2 ml-4 list-disc">
        <li><strong>Cerah</strong> - Langit cerah</li>
        <li><strong>Berawan</strong> - Langit berawan/berkabut</li>
        <li><strong>Hujan</strong> - Hujan/gerimis/badai</li>
      </ul>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
import { useLocationStore } from "@/stores/location";

const locationStore = useLocationStore();
const cityInput = ref("");

const handleUpdateLocation = async () => {
  if (!cityInput.value.trim()) return;

  const success = await locationStore.updateLocation(cityInput.value.trim());

  if (success) {
    cityInput.value = "";
  }
};
</script>
