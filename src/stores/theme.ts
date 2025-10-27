// FILE: src/stores/theme.ts (NEW - Dark Mode Store)
// ============================================
import { defineStore } from "pinia";
import { ref, watch } from "vue";

export const useThemeStore = defineStore("theme", () => {
  const isDark = ref(false);

  // Load theme dari localStorage
  const loadTheme = () => {
    const saved = localStorage.getItem("theme");
    isDark.value = saved === "dark";
    applyTheme();
  };

  // Apply theme ke HTML
  const applyTheme = () => {
    if (isDark.value) {
      document.documentElement.classList.add("dark");
    } else {
      document.documentElement.classList.remove("dark");
    }
  };

  // Toggle theme
  const toggleTheme = () => {
    isDark.value = !isDark.value;
    localStorage.setItem("theme", isDark.value ? "dark" : "light");
    applyTheme();
  };

  // Watch untuk perubahan
  watch(isDark, () => {
    applyTheme();
  });

  return {
    isDark,
    loadTheme,
    toggleTheme,
  };
});
