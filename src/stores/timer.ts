// FILE: src/stores/timer.ts (FIXED - Proper Completion Detection)
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
    completed: false,
    stopped_manually: false,
    remaining_before_stop: 0,
  });

  // Local UI state
  const isRunning = ref(false);
  const isPaused = ref(false);
  const showCompletionMessage = ref(false);
  const showStopMessage = ref(false);
  let currentSessionId = "";
  let completionTimeout: number | null = null;
  let stopTimeout: number | null = null;

  const timerDisplay = computed(() => {
    // Selalu tampilkan dari timerData.remaining (yang di-update oleh ESP32)
    const timeToDisplay = timerData.value.remaining;

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
          isRunning.value = false;
          isPaused.value = false;
          showCompletionMessage.value = false;
          showStopMessage.value = false;

          timerData.value = {
            enabled: false,
            duration: 0,
            remaining: 0,
            completed: false,
            stopped_manually: false,
            remaining_before_stop: 0,
          };

          console.log("🔄 Session changed, timer reset");
        }
      }
    });

    // Listen untuk timer data dari Firebase (di-update oleh ESP32)
    onValue(dbRef(database, "timer"), (snapshot) => {
      if (snapshot.exists()) {
        const newTimerData = snapshot.val();
        const oldTimerData = { ...timerData.value };

        // Update state lokal berdasarkan data dari ESP32
        timerData.value = {
          enabled: newTimerData.enabled || false,
          duration: newTimerData.duration || 0,
          remaining: newTimerData.remaining || 0,
          completed: newTimerData.completed || false,
          stopped_manually: newTimerData.stopped_manually || false,
          remaining_before_stop: newTimerData.remaining_before_stop || 0,
        };

        // Deteksi TIMER COMPLETED (habis natural)
        if (
          newTimerData.completed === true &&
          oldTimerData.completed !== true &&
          newTimerData.remaining === 0
        ) {
          console.log("✅ TIMER COMPLETED NATURALLY!");
          console.log("  Duration:", newTimerData.duration, "seconds");

          isRunning.value = false;
          isPaused.value = false;
          showCompletionMessage.value = true;
          showStopMessage.value = false;

          // Show completion notification
          if (
            "Notification" in window &&
            Notification.permission === "granted"
          ) {
            new Notification("Timer Selesai!", {
              body: `Timer telah selesai setelah ${formatDuration(
                newTimerData.duration
              )}`,
              icon: "/vite.svg",
            });
          }

          // Auto hide message after 5 seconds
          if (completionTimeout) clearTimeout(completionTimeout);
          completionTimeout = window.setTimeout(() => {
            showCompletionMessage.value = false;
          }, 5000);
        }

        // Deteksi TIMER STOPPED MANUALLY
        if (
          newTimerData.stopped_manually === true &&
          oldTimerData.stopped_manually !== true &&
          !newTimerData.enabled
        ) {
          console.log("🛑 TIMER STOPPED MANUALLY!");
          console.log("  Remaining:", oldTimerData.remaining, "seconds");

          isRunning.value = false;
          isPaused.value = false;
          showStopMessage.value = true;
          showCompletionMessage.value = false;

          // Show stop notification
          if (
            "Notification" in window &&
            Notification.permission === "granted"
          ) {
            new Notification("Timer Dihentikan", {
              body: `Timer dihentikan dengan sisa waktu ${formatDuration(
                oldTimerData.remaining
              )}`,
              icon: "/vite.svg",
            });
          }

          // Auto hide message after 3 seconds
          if (stopTimeout) clearTimeout(stopTimeout);
          stopTimeout = window.setTimeout(() => {
            showStopMessage.value = false;
          }, 3000);
        }

        // Update UI state untuk running/paused
        if (newTimerData.enabled) {
          if (newTimerData.remaining > 0) {
            isRunning.value = true;
            isPaused.value = false;
          } else {
            isRunning.value = false;
            isPaused.value = false;
          }
        } else {
          // Timer tidak enabled - bisa completed atau stopped
          if (!newTimerData.completed && !newTimerData.stopped_manually) {
            isRunning.value = false;
            isPaused.value = false;
          }
        }

        // Log untuk debugging (hanya jika ada perubahan signifikan)
        if (
          newTimerData.enabled !== oldTimerData.enabled ||
          Math.abs(newTimerData.remaining - oldTimerData.remaining) >= 5 ||
          newTimerData.completed !== oldTimerData.completed ||
          newTimerData.stopped_manually !== oldTimerData.stopped_manually
        ) {
          console.log("⏱️ Timer update from ESP32:", {
            enabled: newTimerData.enabled,
            duration: newTimerData.duration,
            remaining: newTimerData.remaining,
            completed: newTimerData.completed,
            stopped_manually: newTimerData.stopped_manually,
          });
        }
      }
    });
  };

  const formatDuration = (seconds: number): string => {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;

    const parts = [];
    if (hours > 0) parts.push(`${hours} jam`);
    if (minutes > 0) parts.push(`${minutes} menit`);
    if (secs > 0 || parts.length === 0) parts.push(`${secs} detik`);

    return parts.join(" ");
  };

  const setTimer = async (hours: number, minutes: number, seconds: number) => {
    const totalSeconds = hours * 3600 + minutes * 60 + seconds;

    if (totalSeconds <= 0) {
      console.error("❌ Invalid timer duration");
      return;
    }

    console.log("⏱️ Setting timer:", {
      hours,
      minutes,
      seconds,
      totalSeconds,
    });

    try {
      // Reset flags sebelum start
      await set(dbRef(database, "timer"), {
        enabled: true,
        duration: totalSeconds,
        remaining: totalSeconds,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: 0,
      });

      console.log("✅ Timer sent to ESP32");

      // Reset local messages
      showCompletionMessage.value = false;
      showStopMessage.value = false;

      // Update local state untuk UI feedback
      timerData.value = {
        enabled: true,
        duration: totalSeconds,
        remaining: totalSeconds,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: 0,
      };

      isRunning.value = true;
      isPaused.value = false;
    } catch (error) {
      console.error("❌ Error setting timer:", error);
    }
  };

  const pauseTimer = async () => {
    if (!isRunning.value) return;

    console.log("⏸️ Pausing timer");

    try {
      // Set paused flag di Firebase
      await set(dbRef(database, "timer/enabled"), false);

      isPaused.value = true;
      isRunning.value = false;

      console.log("✅ Timer paused");
    } catch (error) {
      console.error("❌ Error pausing timer:", error);
    }
  };

  const resumeTimer = async () => {
    if (!isPaused.value || timerData.value.remaining <= 0) return;

    console.log(
      "▶️ Resuming timer from:",
      timerData.value.remaining,
      "seconds"
    );

    try {
      // Re-enable timer dengan remaining yang ada
      await set(dbRef(database, "timer"), {
        enabled: true,
        duration: timerData.value.duration,
        remaining: timerData.value.remaining,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: 0,
      });

      isRunning.value = true;
      isPaused.value = false;

      console.log("✅ Timer resumed");
    } catch (error) {
      console.error("❌ Error resuming timer:", error);
    }
  };

  const stopTimer = async () => {
    console.log("🛑 Stopping timer manually");

    try {
      // Mark sebagai manual stop dan reset timer
      await set(dbRef(database, "timer"), {
        enabled: false,
        duration: 0,
        remaining: 0,
        completed: false,
        stopped_manually: true,
        remaining_before_stop: timerData.value.remaining, // Save remaining
      });

      // Reset local state
      isRunning.value = false;
      isPaused.value = false;

      console.log("✅ Timer stopped manually");
    } catch (error) {
      console.error("❌ Error stopping timer:", error);
    }
  };

  const dismissMessages = () => {
    showCompletionMessage.value = false;
    showStopMessage.value = false;
  };

  return {
    timerData,
    timerDisplay,
    isRunning,
    isPaused,
    showCompletionMessage,
    showStopMessage,
    initializeListener,
    setTimer,
    pauseTimer,
    resumeTimer,
    stopTimer,
    dismissMessages,
  };
});
