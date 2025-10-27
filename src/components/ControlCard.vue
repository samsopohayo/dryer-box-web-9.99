// FILE: src/components/ControlCard.vue //
============================================
<template>
  <div class="bg-white rounded-xl shadow-lg p-6">
    <div class="flex items-center justify-between mb-4">
      <div class="flex items-center space-x-3">
        <div class="w-12 h-12 bg-gray-800 rounded-xl"></div>
        <div>
          <h3 class="font-bold text-lg">{{ title }}</h3>
          <p class="text-sm" :class="statusClass">status: {{ status }}</p>
        </div>
      </div>
    </div>

    <button
      @click="handleToggle"
      class="w-full py-3 rounded-lg font-semibold transition"
      :class="buttonClass"
    >
      {{ buttonText }}
    </button>
  </div>
</template>

<script setup lang="ts">
import { computed } from "vue";

const props = defineProps<{
  title: string;
  status: string;
  isOn: boolean;
}>();

const emit = defineEmits<{
  toggle: [];
}>();

const statusClass = computed(() => {
  return props.isOn ? "text-green-600" : "text-gray-600";
});

const buttonClass = computed(() => {
  return props.isOn
    ? "bg-red-500 text-white hover:bg-red-600"
    : "bg-blue-500 text-white hover:bg-blue-600";
});

const buttonText = computed(() => {
  return props.isOn ? "Matikan" : "Hidupkan";
});

const handleToggle = () => {
  emit("toggle");
};
</script>
