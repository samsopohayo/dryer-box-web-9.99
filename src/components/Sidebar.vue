<template>
  <aside
    @mouseenter="handleMouseEnter"
    @mouseleave="handleMouseLeave"
    class="bg-secondary dark:bg-gray-900 text-white fixed left-0 top-0 h-screen transition-all duration-300 z-40 flex flex-col"
    :class="isExpanded ? 'w-64' : 'w-16'"
  >
    <!-- Spacer for header height -->
    <div class="h-[95px]"></div>

    <!-- Navigation Menu -->
    <nav class="flex-1 px-2 py-4 space-y-1 overflow-hidden">
      <router-link
        to="/dashboard"
        @click="handleLinkClick"
        class="flex items-center px-3 py-3 rounded-lg transition group relative"
        :class="
          isActive('/dashboard')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <svg
          class="w-6 h-6 flex-shrink-0"
          :class="
            isActive('/dashboard')
              ? 'text-secondary dark:text-white'
              : 'text-gray-300'
          "
          fill="none"
          stroke="currentColor"
          viewBox="0 0 24 24"
        >
          <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M3 12l2-2m0 0l7-7 7 7M5 10v10a1 1 0 001 1h3m10-11l2 2m-2-2v10a1 1 0 01-1 1h-3m-6 0a1 1 0 001-1v-4a1 1 0 011-1h2a1 1 0 011 1v4a1 1 0 001 1m-6 0h6"
          />
        </svg>
        <span
          class="ml-3 font-medium whitespace-nowrap transition-opacity duration-300"
          :class="isExpanded ? 'opacity-100' : 'opacity-0'"
        >
          Dashboard
        </span>

        <!-- Tooltip for collapsed state -->
        <div
          v-if="!isExpanded && !isHovering"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Dashboard
        </div>
      </router-link>

      <router-link
        to="/control"
        @click="handleLinkClick"
        class="flex items-center px-3 py-3 rounded-lg transition group relative"
        :class="
          isActive('/control')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <svg
          class="w-6 h-6 flex-shrink-0"
          :class="
            isActive('/control')
              ? 'text-secondary dark:text-white'
              : 'text-gray-300'
          "
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
        <span
          class="ml-3 font-medium whitespace-nowrap transition-opacity duration-300"
          :class="isExpanded ? 'opacity-100' : 'opacity-0'"
        >
          Kontrol & Timer
        </span>

        <div
          v-if="!isExpanded && !isHovering"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Kontrol & Timer
        </div>
      </router-link>

      <router-link
        to="/history"
        @click="handleLinkClick"
        class="flex items-center px-3 py-3 rounded-lg transition group relative"
        :class="
          isActive('/history')
            ? 'bg-white text-secondary dark:bg-gray-700 dark:text-white'
            : 'hover:bg-gray-700 dark:hover:bg-gray-800'
        "
      >
        <svg
          class="w-6 h-6 flex-shrink-0"
          :class="
            isActive('/history')
              ? 'text-secondary dark:text-white'
              : 'text-gray-300'
          "
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
        <span
          class="ml-3 font-medium whitespace-nowrap transition-opacity duration-300"
          :class="isExpanded ? 'opacity-100' : 'opacity-0'"
        >
          Rekap Pengeringan
        </span>

        <div
          v-if="!isExpanded && !isHovering"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Rekap Pengeringan
        </div>
      </router-link>
    </nav>

    <!-- User Section (fixed layout -> no jump) -->
    <div class="p-2 border-t border-gray-700">
      <div
        class="mb-3 pb-3 border-b border-gray-700 flex items-center space-x-3"
      >
        <!-- Left: fixed icon column (never moves) -->
        <div
          class="w-10 flex-shrink-0 flex items-center justify-center relative group"
        >
          <div
            class="w-10 h-10 bg-gray-400 dark:bg-gray-600 rounded-full flex items-center justify-center"
          >
            <svg
              class="w-6 h-6 text-gray-600 dark:text-gray-300"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"
              />
            </svg>
          </div>

          <!-- Tooltip saat collapsed (tetap seperti tombol logout) -->
          <div
            v-if="!isExpanded"
            class="absolute left-full top-1/2 -translate-y-1/2 ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
          >
            {{ userEmail }}
          </div>
        </div>

        <!-- Right: info column (kehadirannya tidak mengubah layout karena max-width/overflow) -->
        <div
          :class="[
            'flex-1 min-w-0 transition-all duration-300 ease-in-out overflow-hidden',
            isExpanded ? 'max-w-[400px] opacity-100' : 'max-w-0 opacity-0',
          ]"
          aria-hidden="!isExpanded"
        >
          <p class="text-sm font-medium text-white truncate">
            {{ userEmail }}
          </p>
          <p class="text-xs text-gray-400">Pengguna</p>
        </div>
      </div>

      <!-- Logout Button (tetap seperti semula) -->
      <button
        @click="logout"
        class="w-full flex items-center px-3 py-2 text-red-400 hover:bg-red-500/10 rounded-lg transition group relative overflow-hidden"
      >
        <svg
          class="w-6 h-6 flex-shrink-0"
          fill="none"
          stroke="currentColor"
          viewBox="0 0 24 24"
        >
          <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M17 16l4-4m0 0l-4-4m4 4H7m6 4v1a3 3 0 01-3 3H6a3 3 0 01-3-3V7a3 3 0 013-3h4a3 3 0 013 3v1"
          />
        </svg>
        <span
          class="ml-3 font-medium whitespace-nowrap transition-opacity duration-300"
          :class="isExpanded ? 'opacity-100' : 'opacity-0'"
        >
          Keluar
        </span>

        <div
          v-if="!isExpanded"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Keluar
        </div>
      </button>
    </div>
  </aside>
</template>

<script setup lang="ts">
import { computed, ref } from "vue";
import { useRoute } from "vue-router";
import { useAuthStore } from "@/stores/auth";

const props = defineProps<{
  isCollapsed: boolean;
}>();

const emit = defineEmits<{
  updateCollapsed: [value: boolean];
  hoverStateChange: [isHovering: boolean];
}>();

const route = useRoute();
const authStore = useAuthStore();

const isHovering = ref(false);
const hoverTimer = ref<number | null>(null);

// Computed untuk menentukan apakah sidebar expanded
const isExpanded = computed(() => {
  // Jika sedang hover, expand
  if (isHovering.value) return true;
  // Jika tidak, ikuti props isCollapsed (tapi dibalik karena isCollapsed true = width kecil)
  return !props.isCollapsed;
});

const userEmail = computed(() => authStore.user?.email || "User");

const isActive = (path: string) => {
  return route.path === path;
};

const handleMouseEnter = () => {
  hoverTimer.value = window.setTimeout(() => {
    isHovering.value = true;
  }, 300);
};

const handleMouseLeave = () => {
  if (hoverTimer.value) {
    clearTimeout(hoverTimer.value);
    hoverTimer.value = null;
  }
  // Reset hover state dengan delay kecil untuk transisi yang smooth
  setTimeout(() => {
    isHovering.value = false;
  }, 100);
};

const handleLinkClick = () => {
  // Reset hover state saat link diklik
  isHovering.value = false;
  if (hoverTimer.value) {
    clearTimeout(hoverTimer.value);
    hoverTimer.value = null;
  }
};

const logout = () => {
  authStore.logout();
};
</script>
