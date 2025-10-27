// FILE: src/components/SensorCard.vue //
============================================
<template>
  <div class="bg-white rounded-xl shadow-lg p-6">
    <div class="flex items-center justify-between">
      <div>
        <p class="text-sm text-gray-600 mb-1">{{ label }}</p>
        <p class="text-3xl font-bold text-gray-800">{{ displayValue }}</p>
      </div>
      <div
        class="w-16 h-16 bg-gray-200 rounded-xl flex items-center justify-center"
      >
        <div class="w-12 h-12 bg-gray-800 rounded-lg"></div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from "vue";

const props = defineProps<{
  label: string;
  value: number | string;
  unit: string;
}>();

const displayValue = computed(() => {
  if (typeof props.value === "number") {
    return `${props.value.toFixed(1)}${props.unit ?? ""}`;
  }
  return `${props.value}${props.unit ?? ""}`;
});
</script>
