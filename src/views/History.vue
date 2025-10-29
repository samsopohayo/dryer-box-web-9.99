<!-- History.vue - Updated with Sidebar Toggle -->
<template>
  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar :isCollapsed="isSidebarCollapsed" />
    <div
      class="flex-1 transition-all duration-300"
      :class="isSidebarCollapsed ? 'ml-16' : 'ml-64'"
    >
      <Header @toggleSidebar="toggleSidebar" />
      <main class="p-8">
        <div class="max-w-7xl mx-auto">
          <div
            class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
          >
            <div class="mb-6">
              <label
                class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2"
              >
                Pilih periode pengeringan
              </label>
              <select
                v-model="selectedSession"
                class="w-full md:w-1/3 px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
                @change="loadSessionData"
              >
                <option value="">Semua data</option>
                <option v-for="(session, id) in sessions" :key="id" :value="id">
                  {{ session.session_id }} - {{ session.start_time }}
                </option>
              </select>
            </div>

            <div
              v-if="!selectedSession"
              class="text-center py-12 text-gray-500 dark:text-gray-400"
            >
              <svg
                class="w-16 h-16 mx-auto mb-4 text-gray-400"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  stroke-width="2"
                  d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"
                />
              </svg>
              <p class="text-lg mb-2">data masih kosong</p>
            </div>

            <div v-else-if="isLoading" class="text-center py-12">
              <div
                class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary"
              ></div>
              <p class="mt-4 text-gray-600 dark:text-gray-400">
                Memuat data...
              </p>
            </div>

            <div v-else>
              <!-- Rest of History content remains the same -->
              <!-- Session info, data table, download PDF, delete session -->
            </div>
          </div>
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useDryerStore } from "@/stores/dryer";
import { database } from "@/firebase/config";
import { ref as dbRef, remove } from "firebase/database";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";

const dryerStore = useDryerStore();
const selectedSession = ref("");
const isLoading = ref(false);
const isSidebarCollapsed = ref(false);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const sessions = computed(() => dryerStore.sessions);
// Rest of the script remains the same
</script>
