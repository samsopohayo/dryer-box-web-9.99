// FILE: src/main.ts (UPDATED with theme initialization)
// ============================================
import { createApp } from "vue";
import { createPinia } from "pinia";
import App from "./App.vue";
import router from "./router";
import "./style.css";
import { useThemeStore } from "./stores/theme";

const app = createApp(App);
const pinia = createPinia();

app.use(pinia);
app.use(router);

// Initialize theme
const themeStore = useThemeStore();
themeStore.loadTheme();

app.mount("#app");
