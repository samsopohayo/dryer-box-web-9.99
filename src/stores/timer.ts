// FILE: src/stores/timer.ts (UPDATED - Local Countdown Logic)
// ============================================
import { defineStore } from "pinia";
import { ref, computed, watch } from "vue";
import { database } from "@/firebase/config";
import { ref as dbRef, onValue, set } from "firebase/database";
import type { TimerData } from "@/types";

export const useTimerStore = defineStore("timer", () => {
  const timerData = ref<TimerData>({
    enabled: false,
    duration: 0,
    remaining: 0,
  });

  // Local countdown state
  const localRemaining = ref(0);
  const isRunning = ref(false);
  let countdownInterval: number | null = null;
  let currentSessionId = "";

  const timerDisplay = computed(() => {
    const timeToDisplay = isRunning.value
      ? localRemaining.value
      : timerData.value.remaining;
    const hours = Math.floor(timeToDisplay / 3600);
    const minutes = Math.floor((timeToDisplay % 3600) / 60);
    const seconds = timeToDisplay % 60;
    return `${hours}:${minutes.toString().padStart(2, "0")}:${seconds
      .toString()
      .padStart(2, "0")}`;
  });

  const initializeListener = () => {
    // Listen untuk perubahan session - reset timer jika session baru
    onValue(dbRef(database, "system/current_session"), (snapshot) => {
      if (snapshot.exists()) {
        const newSessionId = snapshot.val();

        // Jika session berubah (session baru dimulai), reset timer
        if (newSessionId && newSessionId !== currentSessionId) {
          currentSessionId = newSessionId;

          // Reset timer state
          if (isRunning.value) {
            stopLocalCountdown();
          }

          localRemaining.value = 0;
          timerData.value = {
            enabled: false,
            duration: 0,
            remaining: 0,
          };
        }
      }
    });

    // Listen untuk timer enabled dari Firebase (untuk stop dari sumber lain)
    onValue(dbRef(database, "timer/enabled"), (snapshot) => {
      if (snapshot.exists()) {
        const enabled = snapshot.val();

        // Jika disabled dari Firebase, stop countdown lokal
        if (!enabled && isRunning.value) {
          stopLocalCountdown();
        }
      }
    });
  };

  const startLocalCountdown = (duration: number) => {
    // Set initial state
    localRemaining.value = duration;
    isRunning.value = true;

    // Clear existing interval jika ada
    if (countdownInterval !== null) {
      clearInterval(countdownInterval);
    }

    // Start countdown interval
    countdownInterval = window.setInterval(() => {
      if (localRemaining.value > 0) {
        localRemaining.value--;

        // Update ke Firebase setiap detik untuk sinkronisasi
        set(dbRef(database, "timer/remaining"), localRemaining.value);
      } else {
        // Timer habis
        stopLocalCountdown();

        // Set enabled ke false di Firebase
        set(dbRef(database, "timer/enabled"), false);
      }
    }, 1000);
  };

  const stopLocalCountdown = () => {
    if (countdownInterval !== null) {
      clearInterval(countdownInterval);
      countdownInterval = null;
    }
    isRunning.value = false;
  };

  const setTimer = async (hours: number, minutes: number, seconds: number) => {
    const totalSeconds = hours * 3600 + minutes * 60 + seconds;

    // Update Firebase
    await set(dbRef(database, "timer"), {
      enabled: true,
      duration: totalSeconds,
      remaining: totalSeconds,
    });

    // Update local state
    timerData.value = {
      enabled: true,
      duration: totalSeconds,
      remaining: totalSeconds,
    };

    // Start local countdown
    startLocalCountdown(totalSeconds);
  };

  const stopTimer = async () => {
    // Stop local countdown
    stopLocalCountdown();

    // Reset local remaining to 0
    localRemaining.value = 0;

    // Update Firebase
    await set(dbRef(database, "timer"), {
      enabled: false,
      duration: 0,
      remaining: 0,
    });

    // Update local state
    timerData.value = {
      enabled: false,
      duration: 0,
      remaining: 0,
    };
  };

  // Cleanup on store disposal
  const cleanup = () => {
    if (countdownInterval !== null) {
      clearInterval(countdownInterval);
    }
  };

  return {
    timerData,
    timerDisplay,
    isRunning,
    localRemaining,
    initializeListener,
    setTimer,
    stopTimer,
    cleanup,
  };
});
