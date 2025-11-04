<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
  >
    <h3
      class="font-bold text-base sm:text-lg mb-4 text-gray-800 dark:text-white"
    >
      Pengaturan Tambahan
    </h3>
    <div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-5 gap-4 sm:gap-6">
      <!-- Target Kelembapan Min -->
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kelembapan Min (%)
        </label>
        <input
          v-model.number="targetHumidityMin"
          type="number"
          min="0"
          max="100"
          step="1"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>

      <!-- Target Kelembapan Max -->
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kelembapan Max (%)
        </label>
        <input
          v-model.number="targetHumidityMax"
          type="number"
          min="0"
          max="100"
          step="1"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>

      <!-- Target Suhu Min -->
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Suhu Min (°C)
        </label>
        <input
          v-model.number="targetTemperatureMin"
          type="number"
          min="0"
          max="100"
          step="1"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>

      <!-- Target Suhu Max -->
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Suhu Max (°C)
        </label>
        <input
          v-model.number="targetTemperatureMax"
          type="number"
          min="0"
          max="100"
          step="1"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>

      <!-- Target Kadar Air -->
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kadar Air (%)
        </label>
        <input
          v-model.number="targetMoisture"
          type="number"
          min="0"
          max="100"
          step="0.1"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>
    </div>

    <!-- Info Box -->
    <div class="mt-6 p-4 bg-blue-50 dark:bg-blue-900 rounded-lg">
      <p class="text-xs text-gray-600 dark:text-gray-300">
        <strong>💡 Catatan:</strong>
      </p>
      <ul
        class="list-disc list-inside mt-2 text-xs text-gray-600 dark:text-gray-300"
      >
        <li>Pastikan nilai Target Kelembapan Min lebih kecil dari Max.</li>
        <li>Pastikan nilai Target Suhu Min lebih kecil dari Max.</li>
        <li>
          Perubahan akan diterapkan secara otomatis setelah Anda mengubah nilai
          pada input di atas.
        </li>
      </ul>
    </div>

    <!-- Current Values Display -->
    <div class="mt-4 grid grid-cols-2 sm:grid-cols-5 gap-3 text-xs">
      <div class="p-3 bg-gray-50 dark:bg-gray-700 rounded-lg text-center">
        <p class="text-gray-600 dark:text-gray-400 mb-1">Kelembapan Min</p>
        <p class="font-bold text-blue-600 dark:text-blue-400">
          {{ targetHumidityMin }}%
        </p>
      </div>
      <div class="p-3 bg-gray-50 dark:bg-gray-700 rounded-lg text-center">
        <p class="text-gray-600 dark:text-gray-400 mb-1">Kelembapan Max</p>
        <p class="font-bold text-blue-600 dark:text-blue-400">
          {{ targetHumidityMax }}%
        </p>
      </div>
      <div class="p-3 bg-gray-50 dark:bg-gray-700 rounded-lg text-center">
        <p class="text-gray-600 dark:text-gray-400 mb-1">Suhu Min</p>
        <p class="font-bold text-red-600 dark:text-red-400">
          {{ targetTemperatureMin }}°C
        </p>
      </div>
      <div class="p-3 bg-gray-50 dark:bg-gray-700 rounded-lg text-center">
        <p class="text-gray-600 dark:text-gray-400 mb-1">Suhu Max</p>
        <p class="font-bold text-red-600 dark:text-red-400">
          {{ targetTemperatureMax }}°C
        </p>
      </div>
      <div class="p-3 bg-gray-50 dark:bg-gray-700 rounded-lg text-center">
        <p class="text-gray-600 dark:text-gray-400 mb-1">Kadar Air Target</p>
        <p class="font-bold text-green-600 dark:text-green-400">
          {{ targetMoisture }}%
        </p>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, watch } from "vue";
import { useDryerStore } from "@/stores/dryer";

const dryerStore = useDryerStore();

// Reactive variables untuk input fields
const targetHumidityMin = ref(dryerStore.configData.targetHumidityMin);
const targetHumidityMax = ref(dryerStore.configData.targetHumidityMax);
const targetTemperatureMin = ref(dryerStore.configData.targetTemperatureMin);
const targetTemperatureMax = ref(dryerStore.configData.targetTemperatureMax);
const targetMoisture = ref(dryerStore.configData.targetMoisture);

// Watch untuk perubahan data dari store (sinkronisasi)
watch(
  () => dryerStore.configData,
  (newConfig) => {
    targetHumidityMin.value = newConfig.targetHumidityMin;
    targetHumidityMax.value = newConfig.targetHumidityMax;
    targetTemperatureMin.value = newConfig.targetTemperatureMin;
    targetTemperatureMax.value = newConfig.targetTemperatureMax;
    targetMoisture.value = newConfig.targetMoisture;
  },
  { deep: true }
);

// Function untuk update config ke Firebase
const updateConfig = async () => {
  // Validasi input
  if (targetHumidityMin.value >= targetHumidityMax.value) {
    alert("Target Kelembapan Min harus lebih kecil dari Max!");
    targetHumidityMin.value = dryerStore.configData.targetHumidityMin;
    return;
  }

  if (targetTemperatureMin.value >= targetTemperatureMax.value) {
    alert("Target Suhu Min harus lebih kecil dari Max!");
    targetTemperatureMin.value = dryerStore.configData.targetTemperatureMin;
    return;
  }

  await dryerStore.updateConfig({
    targetHumidityMin: targetHumidityMin.value,
    targetHumidityMax: targetHumidityMax.value,
    targetTemperatureMin: targetTemperatureMin.value,
    targetTemperatureMax: targetTemperatureMax.value,
    targetMoisture: targetMoisture.value,
  });

  console.log("✅ Config updated:", {
    targetHumidityMin: targetHumidityMin.value,
    targetHumidityMax: targetHumidityMax.value,
    targetTemperatureMin: targetTemperatureMin.value,
    targetTemperatureMax: targetTemperatureMax.value,
    targetMoisture: targetMoisture.value,
  });
};
</script>
