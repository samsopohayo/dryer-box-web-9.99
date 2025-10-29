<!-- Control.vue - Updated -->
<template>
  <!-- Spacer for header height -->
  <div class="h-[95px]"></div>

  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar
      :isCollapsed="isSidebarCollapsed"
      @updateCollapsed="isSidebarCollapsed = $event"
    />

    <div
      class="flex-1 transition-all duration-300"
      :class="isSidebarCollapsed ? 'ml-16' : 'ml-64'"
    >
      <Header @toggleSidebar="toggleSidebar" />
      <main class="p-8">
        <div class="max-w-7xl mx-auto space-y-8">
          <div class="grid grid-cols-1 md:grid-cols-3 gap-6">
            <ControlCard
              title="Heater"
              :status="heaterStatus"
              :isOn="isHeaterOn"
              @toggle="toggleHeater"
            />
            <ControlCard
              title="Fan"
              :status="fanStatus"
              :isOn="isFanOn"
              @toggle="toggleFan"
            />
            <ControlCard
              title="Exhaust"
              :status="exhaustStatus"
              :isOn="isExhaustOn"
              @toggle="toggleExhaust"
            />
          </div>

          <TimerControl />

          <div
            class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
          >
            <h3 class="font-bold text-lg mb-4 text-gray-800 dark:text-white">
              Pengaturan Tambahan
            </h3>
            <div class="grid grid-cols-1 md:grid-cols-3 gap-6">
              <div>
                <label
                  class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
                  >Target Kelembapan Min (%)</label
                >
                <input
                  v-model.number="targetHumidityMin"
                  type="number"
                  class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
                  @change="updateConfig"
                />
              </div>
              <div>
                <label
                  class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
                  >Target Kelembapan Max (%)</label
                >
                <input
                  v-model.number="targetHumidityMax"
                  type="number"
                  class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
                  @change="updateConfig"
                />
              </div>
              <div>
                <label
                  class="block text-sm text-gray-600 dark:text-gray-400 mb-2"
                  >Target Kadar Air (%)</label
                >
                <input
                  v-model.number="targetMoisture"
                  type="number"
                  class="w-full px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
                  @change="updateConfig"
                />
              </div>
            </div>
          </div>

          <LocationSetting />
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch } from "vue";
import { useDryerStore } from "@/stores/dryer";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import ControlCard from "@/components/ControlCard.vue";
import TimerControl from "@/components/TimerControl.vue";
import LocationSetting from "@/components/LocationSetting.vue";

const dryerStore = useDryerStore();
const isSidebarCollapsed = ref(false);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const targetHumidityMin = ref(dryerStore.configData.targetHumidityMin);
const targetHumidityMax = ref(dryerStore.configData.targetHumidityMax);
const targetMoisture = ref(dryerStore.configData.targetMoisture);

watch(
  () => dryerStore.configData,
  (newConfig) => {
    targetHumidityMin.value = newConfig.targetHumidityMin;
    targetHumidityMax.value = newConfig.targetHumidityMax;
    targetMoisture.value = newConfig.targetMoisture;
  },
  { deep: true }
);

const heaterStatus = computed(() =>
  dryerStore.controlData.manual_heater_state ? "on" : "off"
);
const fanStatus = computed(() =>
  dryerStore.controlData.manual_fan_state ? "on" : "off"
);
const exhaustStatus = computed(() =>
  dryerStore.controlData.manual_exhaust_state ? "on" : "off"
);

const isHeaterOn = computed(() => dryerStore.controlData.manual_heater_state);
const isFanOn = computed(() => dryerStore.controlData.manual_fan_state);
const isExhaustOn = computed(() => dryerStore.controlData.manual_exhaust_state);

const toggleHeater = async () => {
  const newState = !dryerStore.controlData.manual_heater_state;
  await dryerStore.updateControl("manual_heater_enable", true);
  await dryerStore.updateControl("manual_heater_state", newState);
};

const toggleFan = async () => {
  const newState = !dryerStore.controlData.manual_fan_state;
  await dryerStore.updateControl("manual_fan_enable", true);
  await dryerStore.updateControl("manual_fan_state", newState);
};

const toggleExhaust = async () => {
  const newState = !dryerStore.controlData.manual_exhaust_state;
  await dryerStore.updateControl("manual_exhaust_enable", true);
  await dryerStore.updateControl("manual_exhaust_state", newState);
};

const updateConfig = async () => {
  await dryerStore.updateConfig({
    targetHumidityMin: targetHumidityMin.value,
    targetHumidityMax: targetHumidityMax.value,
    targetMoisture: targetMoisture.value,
  });
};
</script>
