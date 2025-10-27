// FILE: src/components/TimerControl.vue //
============================================
<template>
  <div class="bg-white rounded-xl shadow-lg p-6">
    <div class="text-center mb-8">
      <h3 class="text-xl font-bold text-gray-800 mb-2">waktu tersisa</h3>
      <p class="text-6xl font-bold text-gray-800 mb-4">{{ displayTime }}</p>
    </div>

    <div class="mb-6">
      <h4 class="font-bold text-gray-800 mb-4">Atur Timer</h4>
      <div class="grid grid-cols-3 gap-4">
        <div>
          <label class="block text-sm text-gray-600 mb-2">Jam</label>
          <select
            v-model="hours"
            class="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent"
          >
            <option v-for="h in 24" :key="h" :value="h - 1">{{ h - 1 }}</option>
          </select>
        </div>
        <div>
          <label class="block text-sm text-gray-600 mb-2">Menit</label>
          <select
            v-model="minutes"
            class="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent"
          >
            <option v-for="m in 60" :key="m" :value="m - 1">{{ m - 1 }}</option>
          </select>
        </div>
        <div>
          <label class="block text-sm text-gray-600 mb-2">Detik</label>
          <select
            v-model="seconds"
            class="w-full px-4 py-2 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent"
          >
            <option v-for="s in 60" :key="s" :value="s - 1">{{ s - 1 }}</option>
          </select>
        </div>
      </div>
    </div>

    <div class="flex space-x-4">
      <button
        @click="startTimer"
        :disabled="timerStore.timerData.enabled"
        class="flex-1 bg-green-500 text-white py-3 rounded-lg font-semibold hover:bg-green-600 transition disabled:opacity-50 disabled:cursor-not-allowed"
      >
        Mulai Timer
      </button>
      <button
        @click="stopTimer"
        :disabled="!timerStore.timerData.enabled"
        class="flex-1 bg-red-500 text-white py-3 rounded-lg font-semibold hover:bg-red-600 transition disabled:opacity-50 disabled:cursor-not-allowed"
      >
        Stop Timer
      </button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useTimerStore } from "@/stores/timer";

const timerStore = useTimerStore();
const hours = ref(0);
const minutes = ref(0);
const seconds = ref(0);

const displayTime = computed(() => {
  return timerStore.timerDisplay;
});

const startTimer = async () => {
  await timerStore.setTimer(hours.value, minutes.value, seconds.value);
};

const stopTimer = async () => {
  await timerStore.stopTimer();
};
</script>
