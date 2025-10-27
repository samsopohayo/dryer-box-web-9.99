<template>
  <div v-if="authStore.loading" style="text-align:center;padding:2rem;">
    Loading...
  </div>
  <router-view v-else />
</template>

<script setup lang="ts">
import { onMounted } from "vue";
import { useDryerStore } from "@/stores/dryer";
import { useTimerStore } from "@/stores/timer";
import { useLocationStore } from "@/stores/location";
import { useNotificationStore } from "@/stores/notification";
import { useAuthStore } from "@/stores/auth";

const dryerStore = useDryerStore();
const timerStore = useTimerStore();
const locationStore = useLocationStore();
const notificationStore = useNotificationStore();
const authStore = useAuthStore();

onMounted(() => {
  dryerStore.initializeListeners();
  timerStore.initializeListener();
  locationStore.initializeListener();
  notificationStore.initializeListener();

  // Request notification permission
  notificationStore.requestPermission();

  // Fetch initial weather
  locationStore.fetchWeather();
});
</script>
