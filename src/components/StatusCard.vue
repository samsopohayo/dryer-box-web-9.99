// FILE: src/components/StatusCard.vue //
============================================
<template>
  <div class="bg-white rounded-xl shadow-lg p-6">
    <div class="flex items-center justify-between">
      <div>
        <p class="text-sm text-gray-600 mb-1">Status</p>
        <p class="text-3xl font-bold" :class="statusColor">{{ status }}</p>
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
  status: string;
}>();

const statusColor = computed(() => {
  const statusLower = props.status.toLowerCase();
  if (statusLower.includes("kering")) return "text-green-600";
  if (statusLower.includes("normal")) return "text-blue-600";
  if (statusLower.includes("lembab")) return "text-yellow-600";
  if (statusLower.includes("proteksi")) return "text-red-600";
  if (statusLower.includes("terbuka")) return "text-orange-600";
  return "text-gray-800";
});
</script>
