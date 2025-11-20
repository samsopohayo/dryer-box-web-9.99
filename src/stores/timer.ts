// FILE: src/stores/timer.ts (FIXED - Web-Driven Timer Logic)
// ============================================
// Web application melakukan SEMUA countdown logic
// ESP32 hanya menerima status dan monitor

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
  let countdownInterval: number | null = null; // ✅ Web melakukan countdown

  const timerDisplay = computed(() => {
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
    // Listen untuk session aktif - reset timer jika session baru
    onValue(dbRef(database, "system/current_session"), (snapshot) => {
      if (snapshot.exists()) {
        const newSessionId = snapshot.val();

        if (newSessionId && newSessionId !== currentSessionId) {
          currentSessionId = newSessionId;

          // Reset timer saat session baru
          stopCountdown();
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

    // Listen untuk timer status dari ESP32 (hanya untuk sync status)
    // Bukan untuk countdown, countdown di-handle di web
    onValue(dbRef(database, "timer"), (snapshot) => {
      if (snapshot.exists()) {
        const espTimer = snapshot.val();

        // ✅ SYNC: Jika ESP32 me-report timer completed, tampilkan message
        if (espTimer.completed === true && !timerData.value.completed) {
          console.log("✅ ESP32 reports: Timer completed!");
          timerData.value.completed = true;
          showCompletionMessage.value = true;

          if (
            "Notification" in window &&
            Notification.permission === "granted"
          ) {
            new Notification("Timer Selesai!", {
              body: `Timer telah selesai setelah ${formatDuration(
                timerData.value.duration
              )}`,
              icon: "/vite.svg",
            });
          }

          if (completionTimeout) clearTimeout(completionTimeout);
          completionTimeout = window.setTimeout(() => {
            showCompletionMessage.value = false;
          }, 5000);
        }

        // ✅ SYNC: Jika ESP32 me-report timer stopped manually
        if (
          espTimer.stopped_manually === true &&
          !timerData.value.stopped_manually
        ) {
          console.log(
            "🛑 ESP32 reports: Timer stopped manually! Remaining:",
            espTimer.remaining_before_stop
          );
          timerData.value.stopped_manually = true;
          timerData.value.remaining_before_stop =
            espTimer.remaining_before_stop || 0;
          showStopMessage.value = true;

          if (
            "Notification" in window &&
            Notification.permission === "granted"
          ) {
            new Notification("Timer Dihentikan", {
              body: `Timer dihentikan dengan sisa waktu ${formatDuration(
                espTimer.remaining_before_stop || 0
              )}`,
              icon: "/vite.svg",
            });
          }

          if (stopTimeout) clearTimeout(stopTimeout);
          stopTimeout = window.setTimeout(() => {
            showStopMessage.value = false;
          }, 3000);
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

  // ✅ Start countdown di web
  const startCountdown = async () => {
    if (countdownInterval) {
      clearInterval(countdownInterval);
    }

    countdownInterval = window.setInterval(async () => {
      if (timerData.value.remaining > 0 && isRunning.value) {
        timerData.value.remaining--;

        // Sync ke Firebase setiap 1 detik agar ESP32 bisa monitor
        await set(
          dbRef(database, "timer/remaining"),
          timerData.value.remaining
        );

        // Deteksi timer habis
        if (timerData.value.remaining === 0) {
          console.log("✅ Timer finished!");

          timerData.value.completed = true;
          isRunning.value = false;

          // Notifikasi ke ESP32 bahwa timer completed
          await set(dbRef(database, "timer"), {
            enabled: false,
            duration: timerData.value.duration,
            remaining: 0,
            completed: true,
            stopped_manually: false,
            remaining_before_stop: 0,
          });

          // Hentikan process pengeringan
          // (ESP32 akan mendeteksi completed flag dan update status proses)

          stopCountdown();
          showCompletionMessage.value = true;

          if (completionTimeout) clearTimeout(completionTimeout);
          completionTimeout = window.setTimeout(() => {
            showCompletionMessage.value = false;
          }, 5000);
        }

        // Log setiap 10 detik
        if (timerData.value.remaining % 10 === 0) {
          console.log("⏱️ Timer:", formatDuration(timerData.value.remaining));
        }
      }
    }, 1000);
  };

  // ✅ Stop countdown
  const stopCountdown = () => {
    if (countdownInterval) {
      clearInterval(countdownInterval);
      countdownInterval = null;
    }
  };

  const setTimer = async (hours: number, minutes: number, seconds: number) => {
    const totalSeconds = hours * 3600 + minutes * 60 + seconds;

    if (totalSeconds <= 0) {
      console.error("❌ Invalid timer duration");
      return;
    }

    console.log("⏱️ Setting timer from WEB:", {
      hours,
      minutes,
      seconds,
      totalSeconds,
    });

    try {
      // Reset flags dan set timer
      timerData.value = {
        enabled: true,
        duration: totalSeconds,
        remaining: totalSeconds,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: 0,
      };

      // Kirim ke Firebase agar ESP32 bisa monitor
      await set(dbRef(database, "timer"), {
        enabled: true,
        duration: totalSeconds,
        remaining: totalSeconds,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: 0,
      });

      console.log("✅ Timer sent to ESP32 and WEB countdown starting");

      // Reset UI messages
      showCompletionMessage.value = false;
      showStopMessage.value = false;

      // ✅ START COUNTDOWN DI WEB
      isRunning.value = true;
      isPaused.value = false;
      await startCountdown();
    } catch (error) {
      console.error("❌ Error setting timer:", error);
    }
  };

  const pauseTimer = async () => {
    if (!isRunning.value) return;

    console.log("⏸️ Pausing timer from WEB");

    try {
      isRunning.value = false;
      isPaused.value = true;
      stopCountdown(); // Stop countdown loop

      // Update Firebase status
      await set(dbRef(database, "timer"), {
        enabled: false, // Disable di ESP32
        duration: timerData.value.duration,
        remaining: timerData.value.remaining,
        completed: false,
        stopped_manually: false,
        remaining_before_stop: timerData.value.remaining,
      });

      console.log("✅ Timer paused, remaining:", timerData.value.remaining);
    } catch (error) {
      console.error("❌ Error pausing timer:", error);
    }
  };

  const resumeTimer = async () => {
    if (!isPaused.value || timerData.value.remaining <= 0) return;

    console.log(
      "▶️ Resuming timer from WEB, remaining:",
      timerData.value.remaining
    );

    try {
      // Update Firebase status
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

      // ✅ RESTART COUNTDOWN DI WEB
      await startCountdown();

      console.log("✅ Timer resumed");
    } catch (error) {
      console.error("❌ Error resuming timer:", error);
    }
  };

  const stopTimer = async () => {
    console.log("🛑 Stopping timer from WEB");

    try {
      stopCountdown(); // Stop countdown loop

      const remainingBeforeStop = timerData.value.remaining;

      // Update Firebase untuk notifikasi ESP32
      await set(dbRef(database, "timer"), {
        enabled: false,
        duration: timerData.value.duration,
        remaining: 0,
        completed: false,
        stopped_manually: true,
        remaining_before_stop: remainingBeforeStop,
      });

      // Reset local state
      isRunning.value = false;
      isPaused.value = false;
      timerData.value.remaining = 0;

      console.log(
        "✅ Timer stopped manually, remaining was:",
        remainingBeforeStop
      );
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
