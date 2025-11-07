// FILE: src/stores/notification.ts (UPDATED - Save to Session Data)
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";
import { database } from "@/firebase/config";
import { ref as dbRef, onValue, get, set, push } from "firebase/database";

interface Notification {
  id: string;
  title: string;
  message: string;
  type: "info" | "warning" | "error" | "success";
  timestamp: number;
  read: boolean;
}

export const useNotificationStore = defineStore("notification", () => {
  const notifications = ref<Notification[]>([]);
  const unreadCount = ref(0);
  const currentSessionId = ref<string>("");

  const initializeListener = () => {
    // Listen untuk session aktif
    onValue(dbRef(database, "system/current_session"), (snapshot) => {
      if (snapshot.exists()) {
        currentSessionId.value = snapshot.val();
      }
    });

    // Listen untuk error dari ESP32
    onValue(dbRef(database, "system/last_error"), (snapshot) => {
      if (snapshot.exists()) {
        const error = snapshot.val();
        if (error !== "None" && error !== "No active errors") {
          addNotification({
            title: "Error Sistem",
            message: error,
            type: "error",
          });
        }
      }
    });

    // Listen untuk door status
    onValue(dbRef(database, "status/door_open"), (snapshot) => {
      if (snapshot.exists() && snapshot.val() === true) {
        addNotification({
          title: "Pintu Terbuka",
          message: "Pintu pengering terbuka, proses dihentikan sementara",
          type: "warning",
        });
      }
    });

    // Listen untuk temperature protection
    onValue(dbRef(database, "status/temp_protection"), (snapshot) => {
      if (snapshot.exists() && snapshot.val() === true) {
        addNotification({
          title: "Proteksi Suhu",
          message: "Suhu terlalu tinggi! Sistem cooling diaktifkan",
          type: "error",
        });
      }
    });

    // Listen untuk process completion
    onValue(dbRef(database, "status/pengeringan"), (snapshot) => {
      if (snapshot.exists() && snapshot.val() === "Selesai") {
        addNotification({
          title: "Proses Selesai",
          message: "Pengeringan telah selesai!",
          type: "success",
        });
      }
    });

    // Listen untuk humidity control
    onValue(dbRef(database, "status/humidity_control"), (snapshot) => {
      if (snapshot.exists() && snapshot.val() === true) {
        addNotification({
          title: "Kontrol Kelembapan Aktif",
          message: "Sistem sedang mengatur kelembapan",
          type: "info",
        });
      }
    });

    // Listen untuk temperature control
    onValue(dbRef(database, "status/temperature_control"), (snapshot) => {
      if (snapshot.exists() && snapshot.val() === true) {
        addNotification({
          title: "Kontrol Suhu Aktif",
          message: "Sistem sedang mengatur suhu",
          type: "info",
        });
      }
    });
  };

  const addNotification = async (data: {
    title: string;
    message: string;
    type: "info" | "warning" | "error" | "success";
  }) => {
    const notification: Notification = {
      id: Date.now().toString(),
      ...data,
      timestamp: Date.now(),
      read: false,
    };

    notifications.value.unshift(notification);
    unreadCount.value++;

    // Save notification to current session data
    await saveNotificationToSession(data.message);

    // Browser notification jika permission granted
    if ("Notification" in window && Notification.permission === "granted") {
      new Notification(notification.title, {
        body: notification.message,
        icon: "/vite.svg",
        badge: "/vite.svg",
      });
    }

    // Auto remove after 10 notifications
    if (notifications.value.length > 10) {
      notifications.value.pop();
    }
  };

  const saveNotificationToSession = async (message: string) => {
    if (!currentSessionId.value) return;

    try {
      // Get current timestamp in Indonesian format
      const now = new Date();
      const day = now.getDate().toString().padStart(2, "0");
      const month = (now.getMonth() + 1).toString().padStart(2, "0");
      const year = now.getFullYear();
      const hours = now.getHours().toString().padStart(2, "0");
      const minutes = now.getMinutes().toString().padStart(2, "0");
      const seconds = now.getSeconds().toString().padStart(2, "0");
      const timestamp = `${day}/${month}/${year} ${hours}:${minutes}:${seconds}`;

      // Cari data point terakhir dengan timestamp yang sama atau paling dekat
      const sessionDataRef = dbRef(
        database,
        `sessions/${currentSessionId.value}/data`
      );
      const snapshot = await get(sessionDataRef);

      if (snapshot.exists()) {
        const dataPoints = snapshot.val();
        let closestKey: string | null = null;
        let closestTimeDiff = Infinity;

        // Cari data point dengan timestamp terdekat (dalam 30 detik terakhir)
        Object.keys(dataPoints).forEach((key) => {
          const dataPoint = dataPoints[key];
          const dataTimestamp = dataPoint.timestamp;

          // Parse timestamps untuk perbandingan
          const parseTimestamp = (ts: string): number => {
            const [date, time] = ts.split(" ");
            const [d, m, y] = date.split("/");
            const [h, min, s] = time.split(":");
            return new Date(
              parseInt(y),
              parseInt(m) - 1,
              parseInt(d),
              parseInt(h),
              parseInt(min),
              parseInt(s)
            ).getTime();
          };

          const dataTime = parseTimestamp(dataTimestamp);
          const currentTime = parseTimestamp(timestamp);
          const timeDiff = Math.abs(currentTime - dataTime);

          // Jika dalam 30 detik terakhir dan lebih dekat
          if (timeDiff < 30000 && timeDiff < closestTimeDiff) {
            closestKey = key;
            closestTimeDiff = timeDiff;
          }
        });

        // Jika menemukan data point terdekat, tambahkan notifikasi
        if (closestKey) {
          const dataPoint = dataPoints[closestKey];
          const existingNotifications = dataPoint.notifications || [];

          // Cek apakah notifikasi sudah ada (hindari duplikasi)
          if (!existingNotifications.includes(message)) {
            existingNotifications.push(message);

            // Update data point dengan notifikasi baru
            await set(
              dbRef(
                database,
                `sessions/${currentSessionId.value}/data/${closestKey}/notifications`
              ),
              existingNotifications
            );

            console.log(
              `✅ Notification saved to session data point: ${closestKey}`
            );
          }
        } else {
          console.log("⚠️ No recent data point found to attach notification");
        }
      }
    } catch (error) {
      console.error("Error saving notification to session:", error);
    }
  };

  const markAsRead = (id: string) => {
    const notif = notifications.value.find((n) => n.id === id);
    if (notif && !notif.read) {
      notif.read = true;
      unreadCount.value--;
    }
  };

  const markAllAsRead = () => {
    notifications.value.forEach((n) => (n.read = true));
    unreadCount.value = 0;
  };

  const clearAll = () => {
    notifications.value = [];
    unreadCount.value = 0;
  };

  const requestPermission = async () => {
    if ("Notification" in window && Notification.permission === "default") {
      await Notification.requestPermission();
    }
  };

  return {
    notifications,
    unreadCount,
    currentSessionId,
    initializeListener,
    addNotification,
    markAsRead,
    markAllAsRead,
    clearAll,
    requestPermission,
  };
});
