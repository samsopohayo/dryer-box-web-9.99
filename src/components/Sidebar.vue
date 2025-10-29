<template>
  <aside
    @mouseenter="handleMouseEnter"
    @mouseleave="handleMouseLeave"
    class="bg-secondary dark:bg-gray-900 text-white fixed left-0 top-0 h-screen transition-all duration-300 z-40 flex flex-col"
    :class="[
      isExpanded ? 'w-64' : 'w-16',
      isMobileMenuOpen ? 'translate-x-0' : 'max-md:-translate-x-full',
    ]"
  >
    <!-- Spacer for header height -->
    <div class="h-[95px]"></div>

    <!-- Navigation Menu -->
    <nav class="flex-1 px-2 py-4 space-y-1 overflow-hidden">
      <router-link
        to="/dashboard"
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

        <!-- Tooltip for collapsed state on desktop -->
        <div
          v-if="!isExpanded && !isMobile"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Dashboard
        </div>
      </router-link>

      <router-link
        to="/control"
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
          v-if="!isExpanded && !isMobile"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Kontrol & Timer
        </div>
      </router-link>

      <router-link
        to="/history"
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
          v-if="!isExpanded && !isMobile"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Rekap Pengeringan
        </div>
      </router-link>
    </nav>

    <!-- User Section -->
    <div class="p-2 border-t border-gray-700">
      <div
        class="mb-3 pb-3 border-b border-gray-700 flex items-center space-x-3"
      >
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

          <div
            v-if="!isExpanded && !isMobile"
            class="absolute left-full top-1/2 -translate-y-1/2 ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
          >
            {{ userEmail }}
          </div>
        </div>

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

      <!-- Logout Button -->
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
          v-if="!isExpanded && !isMobile"
          class="absolute left-full ml-2 px-2 py-1 bg-gray-800 text-white text-sm rounded opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none z-50"
        >
          Keluar
        </div>
      </button>
    </div>
  </aside>

  <!-- Mobile Overlay -->
  <div
    v-if="isMobile && isMobileMenuOpen"
    @click="closeMobileMenu"
    class="fixed inset-0 bg-black bg-opacity-50 z-30 md:hidden"
  ></div>
</template>

<script setup lang="ts">
import { computed, ref, onMounted, onBeforeUnmount, watch } from "vue";
import { useRoute } from "vue-router";
import { useAuthStore } from "@/stores/auth";
import { useSidebarStore } from "@/stores/sidebar";

const props = defineProps<{
  isCollapsed: boolean;
}>();

const emit = defineEmits<{
  updateCollapsed: [value: boolean];
}>();

const route = useRoute();
const authStore = useAuthStore();
const sidebarStore = useSidebarStore();

const isHovering = ref(false);
const isMouseInside = ref(false); // Track apakah mouse masih di dalam sidebar
const hoverTimer = ref<number | null>(null);
const isMobile = ref(false);
const isMobileMenuOpen = ref(false);

// Computed untuk menentukan apakah sidebar expanded
const isExpanded = computed(() => {
  // Di mobile, gunakan state menu mobile
  if (isMobile.value) {
    return isMobileMenuOpen.value;
  }

  // Di desktop:
  // Jika isCollapsed = false (sidebar di-toggle untuk expand), maka expand
  if (!props.isCollapsed) return true;
  // Jika isCollapsed = true DAN (mouse inside ATAU sedang hover), maka expand
  if (isMouseInside.value || isHovering.value) return true;
  // Selain itu, collapsed
  return false;
});

const userEmail = computed(() => authStore.user?.email || "User");

const isActive = (path: string) => {
  return route.path === path;
};

// Detect mobile screen
const checkMobile = () => {
  isMobile.value = window.innerWidth < 768; // md breakpoint

  // Jika berpindah ke desktop dan mobile menu terbuka, tutup
  if (!isMobile.value && isMobileMenuOpen.value) {
    isMobileMenuOpen.value = false;
  }
};

const handleMouseEnter = () => {
  // Hanya aktifkan hover expand jika:
  // 1. Bukan mobile
  // 2. Sidebar dalam kondisi collapsed
  if (isMobile.value || !props.isCollapsed) return;

  // Set bahwa mouse ada di dalam sidebar
  isMouseInside.value = true;

  // Cancel timer sebelumnya jika ada
  if (hoverTimer.value) {
    clearTimeout(hoverTimer.value);
    hoverTimer.value = null;
  }

  // Set delay untuk expand
  hoverTimer.value = window.setTimeout(() => {
    isHovering.value = true;
  }, 200); // Delay 200ms sebelum expand
};

const handleMouseLeave = () => {
  // Set bahwa mouse sudah keluar dari sidebar
  isMouseInside.value = false;

  // Cancel hover timer jika masih berjalan
  if (hoverTimer.value) {
    clearTimeout(hoverTimer.value);
    hoverTimer.value = null;
  }

  // Reset hover state dengan delay kecil untuk transisi smooth
  // PENTING: Ini hanya dijalankan saat mouse benar-benar keluar dari sidebar
  setTimeout(() => {
    // Double check: pastikan mouse memang sudah keluar
    if (!isMouseInside.value) {
      isHovering.value = false;
    }
  }, 100);
};

const closeMobileMenu = () => {
  isMobileMenuOpen.value = false;
};

const logout = () => {
  authStore.logout();
};

// Watch untuk perubahan isCollapsed dari parent (toggle dari header)
watch(
  () => props.isCollapsed,
  (newVal) => {
    // Di mobile, toggle mobile menu state
    if (isMobile.value) {
      isMobileMenuOpen.value = !newVal;
    }

    // Simpan preferensi ke localStorage (hanya untuk desktop)
    if (!isMobile.value) {
      localStorage.setItem("sidebarCollapsed", newVal ? "true" : "false");
    }

    // Reset hover states saat toggle manual
    if (newVal) {
      // Jika di-collapse manual, reset semua hover states
      isHovering.value = false;
      isMouseInside.value = false;
      if (hoverTimer.value) {
        clearTimeout(hoverTimer.value);
        hoverTimer.value = null;
      }
    }
  }
);

// Load preferensi sidebar dari localStorage saat mounted
onMounted(() => {
  checkMobile();
  window.addEventListener("resize", checkMobile);

  // Load saved preference (hanya untuk desktop)
  if (!isMobile.value) {
    const saved = localStorage.getItem("sidebarCollapsed");
    if (saved !== null) {
      const isCollapsed = saved === "true";
      emit("updateCollapsed", isCollapsed);
    }
  }
});

onBeforeUnmount(() => {
  window.removeEventListener("resize", checkMobile);
  if (hoverTimer.value) {
    clearTimeout(hoverTimer.value);
  }
});

// Expose method untuk toggle dari luar (header)
defineExpose({
  toggleMobileMenu: () => {
    if (isMobile.value) {
      isMobileMenuOpen.value = !isMobileMenuOpen.value;
    }
  },
});
</script>
