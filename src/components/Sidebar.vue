// FILE: src/components/Sidebar.vue (UPDATED with dark mode) //
============================================
<template>
  <aside
    class="bg-secondary dark:bg-gray-900 text-white w-64 min-h-screen p-6 transition-colors"
  >
    <div class="mb-8 text-center">
      <div
        class="w-20 h-20 bg-gray-400 dark:bg-gray-600 rounded-full mx-auto mb-4 flex items-center justify-center"
      >
        <span class="text-gray-600 dark:text-gray-300 text-sm">account</span>
      </div>
      <p class="text-sm text-gray-300">{{ userEmail }}</p>
    </div>

    <nav class="space-y-2">
      <router-link
        to="/dashboard"
        class="flex items-center space-x-3 px-4 py-3 rounded-lg transition"
        :class="
          isActive('/dashboard')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <span
          class="w-6 h-6 border-2 rounded"
          :class="isActive('/dashboard') ? 'bg-secondary dark:bg-gray-300' : ''"
        ></span>
        <span class="font-medium">Dashboard</span>
      </router-link>

      <router-link
        to="/control"
        class="flex items-center space-x-3 px-4 py-3 rounded-lg transition"
        :class="
          isActive('/control')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <span
          class="w-6 h-6 border-2 rounded"
          :class="isActive('/control') ? 'bg-secondary dark:bg-gray-300' : ''"
        ></span>
        <span class="font-medium">Kontrol & Timer</span>
      </router-link>

      <router-link
        to="/history"
        class="flex items-center space-x-3 px-4 py-3 rounded-lg transition"
        :class="
          isActive('/history')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <span
          class="w-6 h-6 border-2 rounded"
          :class="isActive('/history') ? 'bg-secondary dark:bg-gray-300' : ''"
        ></span>
        <span class="font-medium">Rekap Pengeringan</span>
      </router-link>
    </nav>
  </aside>
</template>

<script setup lang="ts">
import { computed } from "vue";
import { useRoute } from "vue-router";
import { useAuthStore } from "@/stores/auth";

const route = useRoute();
const authStore = useAuthStore();

const userEmail = computed(() => authStore.user?.email || "User");

const isActive = (path: string) => {
  return route.path === path;
};
</script>
