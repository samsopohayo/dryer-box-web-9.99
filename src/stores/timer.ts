// FILE: src/stores/timer.ts
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

  const timerDisplay = computed(() => {
    const hours = Math.floor(timerData.value.remaining / 3600);
    const minutes = Math.floor((timerData.value.remaining % 3600) / 60);
    const seconds = timerData.value.remaining % 60;
    return `${hours}:${minutes.toString().padStart(2, "0")}:${seconds
      .toString()
      .padStart(2, "0")}`;
  });

  const initializeListener = () => {
    onValue(dbRef(database, "timer"), (snapshot) => {
      if (snapshot.exists()) {
        timerData.value = snapshot.val();
      }
    });
  };

  const setTimer = async (hours: number, minutes: number, seconds: number) => {
    const totalSeconds = hours * 3600 + minutes * 60 + seconds;
    await set(dbRef(database, "timer"), {
      enabled: true,
      duration: totalSeconds,
      remaining: totalSeconds,
    });
  };

  const stopTimer = async () => {
    await set(dbRef(database, "timer/enabled"), false);
  };

  return {
    timerData,
    timerDisplay,
    initializeListener,
    setTimer,
    stopTimer,
  };
});
