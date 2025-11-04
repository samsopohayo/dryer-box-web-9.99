<template>
  <!-- Spacer for header height -->
  <div class="h-[95px]"></div>

  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar
      :isCollapsed="isSidebarCollapsed"
      @updateCollapsed="isSidebarCollapsed = $event"
    />

    <div
      class="flex-1 transition-all duration-300 ease-in-out max-md:ml-0"
      :class="isSidebarCollapsed ? 'md:ml-16' : 'md:ml-64'"
    >
      <Header @toggleSidebar="toggleSidebar" />
      <main class="p-4 sm:p-6 lg:p-8">
        <div class="max-w-7xl mx-auto space-y-6 sm:space-y-8">
          <div class="grid grid-cols-1 md:grid-cols-3 gap-4 sm:gap-6">
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

          <AdditionalSettings />

          <LocationSetting />
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useDryerStore } from "@/stores/dryer";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import ControlCard from "@/components/ControlCard.vue";
import TimerControl from "@/components/TimerControl.vue";
import AdditionalSettings from "@/components/AdditionalSettings.vue";
import LocationSetting from "@/components/LocationSetting.vue";

const dryerStore = useDryerStore();
const isSidebarCollapsed = ref(true);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

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
</script>
