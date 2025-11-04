<script setup lang="ts">
import { ref } from "vue";
import { useDryerStore } from "@/stores/dryer";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import InformationCard from "@/components/InformationCard.vue";
import SensorCard from "@/components/SensorCard.vue";
import StatusCard from "@/components/StatusCard.vue";
import ChartMonitoring from "@/components/ChartMonitoring.vue";

const dryerStore = useDryerStore();
const isSidebarCollapsed = ref(true);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};
</script>

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
        <div class="max-w-7xl mx-auto">
          <!-- Sensor Cards -->
          <div
            class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6 mb-6 sm:mb-8"
          >
            <SensorCard
              label="Suhu"
              :value="dryerStore.sensorData.suhu"
              unit="°C"
            />
            <SensorCard
              label="Kelembapan"
              :value="dryerStore.sensorData.humidity"
              unit="%"
            />
            <SensorCard
              label="Kadar Air"
              :value="dryerStore.sensorData.kadarAir"
              unit="%"
            />
            <SensorCard
              label="Berat"
              :value="dryerStore.sensorData.berat"
              unit=" g"
            />
            <StatusCard :status="dryerStore.overallStatus" />
            <SensorCard
              label="Status Proses"
              :value="dryerStore.statusData.pengeringan"
              unit=""
            />
          </div>

          <!-- Chart -->
          <div
            class="mt-6 sm:mt-8 bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
          >
            <ChartMonitoring />
          </div>

          <!-- System Information -->
          <InformationCard />
        </div>
      </main>
    </div>
  </div>
</template>
