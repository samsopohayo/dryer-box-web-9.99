<template>
  <div
    class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
  >
    <h3
      class="font-bold text-base sm:text-lg mb-4 text-gray-800 dark:text-white"
    >
      Pengaturan Tambahan
    </h3>
    <div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6">
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kelembapan Min (%)
        </label>
        <input
          v-model.number="targetHumidityMin"
          type="number"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kelembapan Max (%)
        </label>
        <input
          v-model.number="targetHumidityMax"
          type="number"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
      </div>
      <div>
        <label class="block text-sm text-gray-600 dark:text-gray-400 mb-2">
          Target Kadar Air (%)
        </label>
        <input
          v-model.number="targetMoisture"
          type="number"
          class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
          @change="updateConfig"
        />
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
const targetMoisture = ref(dryerStore.configData.targetMoisture);

// Watch untuk perubahan data dari store (sinkronisasi)
watch(
  () => dryerStore.configData,
  (newConfig) => {
    targetHumidityMin.value = newConfig.targetHumidityMin;
    targetHumidityMax.value = newConfig.targetHumidityMax;
    targetMoisture.value = newConfig.targetMoisture;
  },
  { deep: true }
);

// Function untuk update config ke Firebase
const updateConfig = async () => {
  await dryerStore.updateConfig({
    targetHumidityMin: targetHumidityMin.value,
    targetHumidityMax: targetHumidityMax.value,
    targetMoisture: targetMoisture.value,
  });
};
</script>
