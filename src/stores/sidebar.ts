// FILE: src/stores/sidebar.ts (NEW - Sidebar State Store)
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";

export const useSidebarStore = defineStore("sidebar", () => {
  const isCollapsed = ref(false);

  const toggleSidebar = () => {
    isCollapsed.value = !isCollapsed.value;
  };

  const setSidebarState = (collapsed: boolean) => {
    isCollapsed.value = collapsed;
  };

  return {
    isCollapsed,
    toggleSidebar,
    setSidebarState,
  };
});
