// FILE: src/stores/timer.ts (UPDATED - With Pause/Resume)
// ============================================
import { defineStore } from "pinia";
import { ref, computed } from "vue";
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
  const isPaused = ref(false);
  let countdownInterval: number | null = null;
  let currentSessionId = "";

  const timerDisplay = computed(() => {
    const timeToDisplay =
      isRunning.value || isPaused.value
        ? localRemaining.value
        : timerData.value.remaining;

    const days = Math.floor(timeToDisplay / 86400);
    const hours = Math.floor((timeToDisplay % 86400) / 3600);
    const minutes = Math.floor((timeToDisplay % 3600) / 60);
    const seconds = timeToDisplay % 60;

    if (days > 0) {
      return `${days}d ${hours.toString().padStart(2, "0")}:${minutes
        .toString()
        .padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
    }

    return `${hours.toString().padStart(2, "0")}:${minutes
      .toString()
      .padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
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
          isPaused.value = false;
          timerData.value = {
            enabled: false,
            duration: 0,
            remaining: 0,
          };
        }
      }
    });

    // Listen untuk timer enabled dari Firebase
    onValue(dbRef(database, "timer/enabled"), (snapshot) => {
      if (snapshot.exists()) {
        const enabled = snapshot.val();

        // Jika timer diaktifkan dari ESP32, mulai countdown lokal
        if (enabled && !isRunning.value && !isPaused.value) {
          // Ambil duration dan remaining dari Firebase
          const remaining = timerData.value.remaining;

          if (remaining > 0) {
            startLocalCountdown(remaining);
          }
        }

        // Jika disabled dari Firebase, stop countdown lokal
        if (!enabled && (isRunning.value || isPaused.value)) {
          stopLocalCountdown();
          isPaused.value = false;
        }
      }
    });

    // Listen untuk perubahan timer data
    onValue(dbRef(database, "timer"), (snapshot) => {
      if (snapshot.exists()) {
        const newTimerData = snapshot.val();

        // Update timerData hanya jika timer tidak sedang berjalan lokal
        if (!isRunning.value && !isPaused.value) {
          timerData.value = newTimerData;
        }
      }
    });
  };

  const startLocalCountdown = (duration: number) => {
    // Set initial state
    localRemaining.value = duration;
    isRunning.value = true;
    isPaused.value = false;

    // Clear existing interval jika ada
    if (countdownInterval !== null) {
      clearInterval(countdownInterval);
    }

    console.log("🕐 Starting local countdown:", duration, "seconds");

    // Start countdown interval
    countdownInterval = window.setInterval(() => {
      if (localRemaining.value > 0) {
        localRemaining.value--;

        // Update ke Firebase setiap detik untuk sinkronisasi
        set(dbRef(database, "timer/remaining"), localRemaining.value);
      } else {
        // Timer habis
        console.log("⏰ Timer completed!");
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
    console.log("⏸️ Local countdown stopped");
  };

  const pauseTimer = () => {
    if (isRunning.value) {
      stopLocalCountdown();
      isPaused.value = true;

      // Update Firebase dengan status paused (tetap enabled tapi tidak countdown)
      set(dbRef(database, "timer/remaining"), localRemaining.value);

      console.log("⏸️ Timer paused at:", localRemaining.value, "seconds");
    }
  };

  const resumeTimer = () => {
    if (isPaused.value && localRemaining.value > 0) {
      console.log("▶️ Resuming timer from:", localRemaining.value, "seconds");
      startLocalCountdown(localRemaining.value);
    }
  };

  const setTimer = async (hours: number, minutes: number, seconds: number) => {
    const totalSeconds = hours * 3600 + minutes * 60 + seconds;

    console.log("⏱️ Setting timer:", { hours, minutes, seconds, totalSeconds });

    // Update Firebase - ESP32 akan membaca ini dan memulai timer
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
    console.log("🛑 Stopping timer");

    // Stop local countdown
    stopLocalCountdown();

    // Reset local remaining to 0
    localRemaining.value = 0;
    isPaused.value = false;

    // Update Firebase - ESP32 akan membaca ini dan menghentikan timer
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
    isPaused,
    localRemaining,
    initializeListener,
    setTimer,
    pauseTimer,
    resumeTimer,
    stopTimer,
    cleanup,
  };
});
