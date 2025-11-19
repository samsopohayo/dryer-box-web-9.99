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
          <!-- Control Mode Toggle -->
          <div
            class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
          >
            <div class="flex items-center justify-between mb-4">
              <div>
                <h3 class="font-bold text-lg text-gray-800 dark:text-white">
                  Mode Kontrol
                </h3>
                <p class="text-sm text-gray-600 dark:text-gray-400 mt-1">
                  {{
                    isAutoMode
                      ? "Sistem menggunakan Fuzzy Logic otomatis"
                      : "Kontrol manual aktif"
                  }}
                </p>
              </div>

              <div class="flex items-center space-x-4">
                <span
                  class="text-sm font-medium"
                  :class="
                    isAutoMode
                      ? 'text-blue-600 dark:text-blue-400'
                      : 'text-gray-500 dark:text-gray-400'
                  "
                >
                  AUTO
                </span>
                <button
                  @click="toggleControlMode"
                  :disabled="!isConnected"
                  class="relative inline-flex h-8 w-16 items-center rounded-full transition-colors focus:outline-none focus:ring-2 focus:ring-primary focus:ring-offset-2 disabled:opacity-50 disabled:cursor-not-allowed"
                  :class="
                    isAutoMode
                      ? 'bg-blue-600'
                      : 'bg-orange-500 dark:bg-orange-600'
                  "
                >
                  <span
                    class="inline-block h-6 w-6 transform rounded-full bg-white transition-transform"
                    :class="isAutoMode ? 'translate-x-1' : 'translate-x-9'"
                  />
                </button>
                <span
                  class="text-sm font-medium"
                  :class="
                    !isAutoMode
                      ? 'text-orange-600 dark:text-orange-400'
                      : 'text-gray-500 dark:text-gray-400'
                  "
                >
                  MANUAL
                </span>
              </div>
            </div>

            <!-- Mode Description -->
            <div
              class="p-4 rounded-lg"
              :class="
                isAutoMode
                  ? 'bg-blue-50 dark:bg-blue-900'
                  : 'bg-orange-50 dark:bg-orange-900'
              "
            >
              <div class="flex items-start space-x-3">
                <svg
                  v-if="isAutoMode"
                  class="w-5 h-5 text-blue-600 dark:text-blue-400 mt-0.5"
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
                <svg
                  v-else
                  class="w-5 h-5 text-orange-600 dark:text-orange-400 mt-0.5"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                >
                  <path
                    stroke-linecap="round"
                    stroke-linejoin="round"
                    stroke-width="2"
                    d="M12 6V4m0 2a2 2 0 100 4m0-4a2 2 0 110 4m-6 8a2 2 0 100-4m0 4a2 2 0 110-4m0 4v2m0-6V4m6 6v10m6-2a2 2 0 100-4m0 4a2 2 0 110-4m0 4v2m0-6V4"
                  />
                </svg>
                <div class="flex-1">
                  <p
                    class="text-sm font-semibold"
                    :class="
                      isAutoMode
                        ? 'text-blue-800 dark:text-blue-200'
                        : 'text-orange-800 dark:text-orange-200'
                    "
                  >
                    {{ isAutoMode ? "Mode AUTO" : "Mode MANUAL" }}
                  </p>
                  <p
                    class="text-xs mt-1"
                    :class="
                      isAutoMode
                        ? 'text-blue-700 dark:text-blue-300'
                        : 'text-orange-700 dark:text-orange-300'
                    "
                  >
                    {{
                      isAutoMode
                        ? "Sistem akan mengatur suhu, kelembapan, dan kipas secara otomatis menggunakan algoritma Fuzzy Logic berdasarkan kondisi sensor."
                        : "Anda dapat mengontrol perangkat secara manual melalui tombol toggle atau menggunakan timer untuk mengatur durasi operasi."
                    }}
                  </p>
                </div>
              </div>
            </div>
          </div>

          <!-- Manual Controls (Only show in MANUAL mode) -->
          <div v-if="!isAutoMode && isConnected">
            <div
              class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6"
            >
              <ControlCard
                title="Heater"
                device="heater"
                @toggle="toggleHeater"
              />
              <ControlCard
                title="Fan Collector"
                device="fan_collector"
                @toggle="toggleFanCollector"
              />
              <ControlCard
                title="Fan Panel"
                device="fan_panel"
                @toggle="toggleFanPanel"
              />
              <ControlCard
                title="Exhaust"
                device="exhaust"
                @toggle="toggleExhaust"
              />
            </div>

            <!-- Timer Control (Only in MANUAL mode) -->
            <div class="mt-6">
              <TimerControl />
            </div>
          </div>

          <!-- Auto Mode Info -->
          <div v-if="isAutoMode && isConnected">
            <div
              class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 sm:gap-6"
            >
              <ControlCard title="Heater" device="heater" />
              <ControlCard title="Fan Collector" device="fan_collector" />
              <ControlCard title="Fan Panel" device="fan_panel" />
              <ControlCard title="Exhaust" device="exhaust" />
            </div>
          </div>

          <!-- Additional Settings -->
          <AdditionalSettings />

          <!-- Location Setting -->
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

// Check if system is connected (last update within 1 minute)
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

// Control mode state
const isAutoMode = computed(() => {
  return dryerStore.controlData.auto_mode !== false; // Default true if not set
});

// Toggle control mode
const toggleControlMode = async () => {
  if (!isConnected.value) return;

  const newMode = !isAutoMode.value;
  await dryerStore.updateControl("auto_mode", newMode);

  console.log("🔄 Control mode changed to:", newMode ? "AUTO" : "MANUAL");
};

// Manual control toggle functions
const toggleHeater = async () => {
  const newState = !dryerStore.controlData.manual_heater_state;
  await dryerStore.updateControl("manual_heater_enable", true);
  await dryerStore.updateControl("manual_heater_state", newState);
};

const toggleFanCollector = async () => {
  const newState = !dryerStore.controlData.manual_fan_collector_state;
  await dryerStore.updateControl("manual_fan_collector_enable", true);
  await dryerStore.updateControl("manual_fan_collector_state", newState);
};

const toggleFanPanel = async () => {
  const newState = !dryerStore.controlData.manual_fan_panel_state;
  await dryerStore.updateControl("manual_fan_panel_enable", true);
  await dryerStore.updateControl("manual_fan_panel_state", newState);
};

const toggleExhaust = async () => {
  const newState = !dryerStore.controlData.manual_exhaust_state;
  await dryerStore.updateControl("manual_exhaust_enable", true);
  await dryerStore.updateControl("manual_exhaust_state", newState);
};
</script>
