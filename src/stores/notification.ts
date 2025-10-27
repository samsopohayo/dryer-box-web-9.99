// FILE: src/stores/notification.ts (NEW - Push Notification Store)
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";
import { database } from "@/firebase/config";
import { ref as dbRef, onValue } from "firebase/database";

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

  const initializeListener = () => {
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
  };

  const addNotification = (data: {
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
    initializeListener,
    addNotification,
    markAsRead,
    markAllAsRead,
    clearAll,
    requestPermission,
  };
});
