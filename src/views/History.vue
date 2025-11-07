<template>
  <!-- Spacer for header height -->
  <div class="h-[95px]"></div>

  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar
      :isCollapsed="isSidebarCollapsed"
      @updateCollapsed="isSidebarCollapsed = $event"
    />

    <div
      class="flex-1 transition-all duration-300 ease-in-out max-md:ml-0"
      :class="isSidebarCollapsed ? 'md:ml-16' : 'md:ml-64'"
    >
      <Header @toggleSidebar="toggleSidebar" />

      <main class="p-4 sm:p-6 lg:p-8">
        <div class="max-w-7xl mx-auto">
          <div
            class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4 sm:p-6 transition-colors"
          >
            <!-- Session Selector -->
            <div class="mb-6">
              <label
                class="block text-sm font-medium text-gray-700 dark:text-gray-300 mb-2"
              >
                Pilih periode pengeringan
              </label>
              <select
                v-model="selectedSession"
                class="w-full md:w-1/3 px-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white"
                @change="loadSessionData"
              >
                <option value="">-- Pilih session --</option>
                <option v-for="(session, id) in sessions" :key="id" :value="id">
                  {{ session.session_id }} - {{ session.start_time }}
                </option>
              </select>
            </div>

            <!-- Empty State -->
            <div
              v-if="!selectedSession"
              class="text-center py-12 text-gray-500 dark:text-gray-400"
            >
              <svg
                class="w-16 h-16 mx-auto mb-4 text-gray-400"
                fill="none"
                stroke="currentColor"
                viewBox="0 0 24 24"
              >
                <path
                  stroke-linecap="round"
                  stroke-linejoin="round"
                  stroke-width="2"
                  d="M9 12h6m-6 4h6m2 5H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"
                />
              </svg>
              <p class="text-base sm:text-lg mb-2">
                Silakan pilih session pengeringan
              </p>
              <p class="text-xs sm:text-sm">
                Total sesi tersedia: {{ Object.keys(sessions).length }}
              </p>
            </div>

            <!-- Loading State -->
            <div v-else-if="isLoading" class="text-center py-12">
              <div
                class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary"
              ></div>
              <p class="mt-4 text-gray-600 dark:text-gray-400">
                Memuat data...
              </p>
            </div>

            <!-- Data Display -->
            <div v-else>
              <!-- Session Info Cards - Responsive Grid -->
              <div
                class="mb-6 grid grid-cols-2 lg:grid-cols-4 gap-3 sm:gap-4 text-xs sm:text-sm"
              >
                <div
                  class="bg-gray-50 dark:bg-gray-700 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-400 truncate">
                    Session ID
                  </p>
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
                  >
                    {{ currentSession?.session_id || "-" }}
                  </p>
                </div>
                <div
                  class="bg-gray-50 dark:bg-gray-700 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-400 truncate">
                    Waktu Mulai
                  </p>
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
                  >
                    {{ formatDateTime(currentSession?.start_time) }}
                  </p>
                </div>
                <div
                  class="bg-gray-50 dark:bg-gray-700 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-400 truncate">
                    Waktu Selesai
                  </p>
                  <p
                    class="font-semibold text-gray-800 dark:text-white truncate"
                  >
                    {{
                      currentSession?.end_time
                        ? formatDateTime(currentSession.end_time)
                        : "Berjalan"
                    }}
                  </p>
                </div>
                <div
                  class="bg-gray-50 dark:bg-gray-700 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-400 truncate">
                    Status
                  </p>
                  <p
                    class="font-semibold capitalize text-gray-800 dark:text-white truncate"
                  >
                    {{ currentSession?.status || "-" }}
                  </p>
                </div>
              </div>

              <!-- Weight & Moisture Cards - Responsive Grid -->
              <div
                class="mb-6 grid grid-cols-1 sm:grid-cols-3 gap-3 sm:gap-4 text-xs sm:text-sm"
              >
                <div
                  class="bg-blue-50 dark:bg-blue-900 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-300 truncate">
                    Berat Awal
                  </p>
                  <p
                    class="text-xl sm:text-2xl font-bold text-blue-600 dark:text-blue-300 truncate"
                  >
                    {{ currentSession?.initial_weight?.toFixed(2) || "0" }} g
                  </p>
                </div>
                <div
                  class="bg-green-50 dark:bg-green-900 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-300 truncate">
                    Berat Akhir
                  </p>
                  <p
                    class="text-xl sm:text-2xl font-bold text-green-600 dark:text-green-300 truncate"
                  >
                    {{ currentSession?.final_weight?.toFixed(2) || "-" }} g
                  </p>
                </div>
                <div
                  class="bg-purple-50 dark:bg-purple-900 p-3 sm:p-4 rounded-lg overflow-hidden"
                >
                  <p class="text-gray-600 dark:text-gray-300 truncate">
                    Kadar Air Akhir
                  </p>
                  <p
                    class="text-xl sm:text-2xl font-bold text-purple-600 dark:text-purple-300 truncate"
                  >
                    {{ currentSession?.final_moisture?.toFixed(2) || "-" }} %
                  </p>
                </div>
              </div>

              <!-- Data Points Count & Notification Summary -->
              <div class="mb-4 flex flex-wrap gap-4 text-xs sm:text-sm">
                <div class="text-gray-600 dark:text-gray-400">
                  Total data points:
                  <span class="font-semibold">{{ dataPointsCount }}</span>
                </div>
                <div class="text-gray-600 dark:text-gray-400">
                  Total notifikasi:
                  <span
                    class="font-semibold text-orange-600 dark:text-orange-400"
                  >
                    {{ totalNotifications }}
                  </span>
                </div>
              </div>

              <!-- Table - Desktop -->
              <div
                class="overflow-hidden rounded-lg border border-gray-200 dark:border-gray-700"
              >
                <div class="hidden lg:block overflow-x-auto">
                  <table class="w-full">
                    <thead class="bg-gray-50 dark:bg-gray-700">
                      <tr>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          No
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Waktu
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Suhu (°C)
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Humidity (%)
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Kadar Air (%)
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Berat (g)
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Status
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Heater
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Fan
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                        >
                          Exhaust
                        </th>
                        <th
                          class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider min-w-[200px]"
                        >
                          Keterangan
                        </th>
                      </tr>
                    </thead>
                    <tbody
                      class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700"
                    >
                      <tr v-if="dataPointsCount === 0">
                        <td
                          colspan="11"
                          class="px-4 py-8 text-center text-gray-500 dark:text-gray-400"
                        >
                          Tidak ada data untuk session ini
                        </td>
                      </tr>
                      <tr
                        v-for="(data, index) in sessionDataPoints"
                        :key="index"
                        class="hover:bg-gray-50 dark:hover:bg-gray-700 transition"
                      >
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ index + 1 }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300 whitespace-nowrap"
                        >
                          {{ data.timestamp }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.temperature.toFixed(1) }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.humidity.toFixed(1) }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.moisture.toFixed(1) }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.weight.toFixed(1) }}
                        </td>
                        <td class="px-4 py-3 text-sm">
                          <span
                            class="px-2 py-1 rounded-full text-xs font-semibold whitespace-nowrap"
                            :class="getStatusClass(data.status)"
                          >
                            {{ data.status }}
                          </span>
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.heater }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.fan }}
                        </td>
                        <td
                          class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                        >
                          {{ data.exhaust }}
                        </td>
                        <td class="px-4 py-3 text-sm">
                          <div
                            v-if="
                              data.notifications &&
                              data.notifications.length > 0
                            "
                            class="space-y-1"
                          >
                            <div
                              v-for="(notif, nIdx) in data.notifications"
                              :key="nIdx"
                              class="text-xs"
                              :class="getNotificationColor(notif)"
                            >
                              {{ notif }}
                            </div>
                          </div>
                          <span
                            v-else
                            class="text-gray-400 dark:text-gray-500 text-xs"
                            >-</span
                          >
                        </td>
                      </tr>
                    </tbody>
                  </table>
                </div>

                <!-- Mobile/Tablet Cards View -->
                <div
                  class="lg:hidden divide-y divide-gray-200 dark:divide-gray-700"
                >
                  <div
                    v-if="dataPointsCount === 0"
                    class="p-6 text-center text-gray-500 dark:text-gray-400"
                  >
                    Tidak ada data untuk session ini
                  </div>

                  <div
                    v-for="(data, index) in sessionDataPoints"
                    :key="index"
                    class="p-4 bg-white dark:bg-gray-800 hover:bg-gray-50 dark:hover:bg-gray-700 transition"
                  >
                    <!-- Card Header -->
                    <div
                      class="flex items-center justify-between mb-3 pb-3 border-b border-gray-200 dark:border-gray-700"
                    >
                      <div class="flex items-center space-x-2">
                        <span
                          class="text-xs font-semibold text-gray-500 dark:text-gray-400"
                        >
                          #{{ index + 1 }}
                        </span>
                        <span
                          class="px-2 py-1 rounded-full text-xs font-semibold"
                          :class="getStatusClass(data.status)"
                        >
                          {{ data.status }}
                        </span>
                      </div>
                      <div class="text-xs text-gray-500 dark:text-gray-400">
                        {{ data.timestamp }}
                      </div>
                    </div>

                    <!-- Card Body - Two Column Grid -->
                    <div class="grid grid-cols-2 gap-3 mb-3">
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Suhu
                        </p>
                        <p
                          class="text-sm font-semibold text-red-600 dark:text-red-400"
                        >
                          {{ data.temperature.toFixed(1) }}°C
                        </p>
                      </div>
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Humidity
                        </p>
                        <p
                          class="text-sm font-semibold text-blue-600 dark:text-blue-400"
                        >
                          {{ data.humidity.toFixed(1) }}%
                        </p>
                      </div>
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Kadar Air
                        </p>
                        <p
                          class="text-sm font-semibold text-green-600 dark:text-green-400"
                        >
                          {{ data.moisture.toFixed(1) }}%
                        </p>
                      </div>
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Berat
                        </p>
                        <p
                          class="text-sm font-semibold text-purple-600 dark:text-purple-400"
                        >
                          {{ data.weight.toFixed(1) }} g
                        </p>
                      </div>
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Heater
                        </p>
                        <p
                          class="text-sm font-semibold text-gray-800 dark:text-gray-200"
                        >
                          {{ data.heater }}
                        </p>
                      </div>
                      <div>
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Fan
                        </p>
                        <p
                          class="text-sm font-semibold text-gray-800 dark:text-gray-200"
                        >
                          {{ data.fan }}
                        </p>
                      </div>
                      <div class="col-span-2">
                        <p class="text-xs text-gray-500 dark:text-gray-400">
                          Exhaust
                        </p>
                        <p
                          class="text-sm font-semibold text-gray-800 dark:text-gray-200"
                        >
                          {{ data.exhaust }}
                        </p>
                      </div>
                    </div>

                    <!-- Notifications Section (Mobile) -->
                    <div
                      v-if="data.notifications && data.notifications.length > 0"
                      class="pt-3 border-t border-gray-200 dark:border-gray-700"
                    >
                      <p
                        class="text-xs text-gray-500 dark:text-gray-400 mb-2 font-semibold"
                      >
                        Keterangan:
                      </p>
                      <div class="space-y-2">
                        <div
                          v-for="(notif, nIdx) in data.notifications"
                          :key="nIdx"
                          class="p-2 rounded bg-gray-50 dark:bg-gray-700"
                        >
                          <span
                            class="text-xs"
                            :class="getNotificationColor(notif)"
                          >
                            {{ notif }}
                          </span>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <!-- Action Buttons - Responsive -->
              <div class="mt-6 flex flex-col sm:flex-row gap-3 sm:gap-4">
                <button
                  @click="downloadPDF"
                  :disabled="dataPointsCount === 0"
                  class="w-full sm:flex-1 px-4 sm:px-6 py-3 bg-blue-500 text-white rounded-lg font-semibold hover:bg-blue-600 transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
                >
                  <svg
                    class="w-5 h-5"
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      stroke-linecap="round"
                      stroke-linejoin="round"
                      stroke-width="2"
                      d="M12 10v6m0 0l-3-3m3 3l3-3m2 8H7a2 2 0 01-2-2V5a2 2 0 012-2h5.586a1 1 0 01.707.293l5.414 5.414a1 1 0 01.293.707V19a2 2 0 01-2 2z"
                    />
                  </svg>
                  <span>Download PDF</span>
                </button>
                <button
                  @click="deleteSession"
                  class="w-full sm:flex-1 px-4 sm:px-6 py-3 bg-red-500 text-white rounded-lg font-semibold hover:bg-red-600 transition flex items-center justify-center space-x-2"
                >
                  <svg
                    class="w-5 h-5"
                    fill="none"
                    stroke="currentColor"
                    viewBox="0 0 24 24"
                  >
                    <path
                      stroke-linecap="round"
                      stroke-linejoin="round"
                      stroke-width="2"
                      d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"
                    />
                  </svg>
                  <span>Hapus Data</span>
                </button>
              </div>

              <!-- Info Note -->
              <div
                class="mt-4 p-3 sm:p-4 bg-yellow-50 dark:bg-yellow-900 rounded-lg"
              >
                <p class="text-xs text-gray-700 dark:text-gray-300">
                  <strong>💡 Catatan:</strong> Data rekap akan otomatis
                  tersimpan setiap 15 detik selama proses pengeringan
                  berlangsung. Kolom keterangan menampilkan notifikasi sistem
                  yang terjadi pada waktu data point tersebut. Export PDF akan
                  menyertakan semua data termasuk keterangan.
                </p>
              </div>
            </div>
          </div>
        </div>
      </main>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useDryerStore } from "@/stores/dryer";
import { database } from "@/firebase/config";
import { ref as dbRef, remove } from "firebase/database";
import Header from "@/components/Header.vue";
import Sidebar from "@/components/Sidebar.vue";
import jsPDF from "jspdf";
import autoTable from "jspdf-autotable";

const dryerStore = useDryerStore();
const selectedSession = ref("");
const isLoading = ref(false);
const isSidebarCollapsed = ref(true);

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const sessions = computed(() => {
  const allSessions: Record<string, any> = {};
  const rawSessions = dryerStore.sessions;

  Object.keys(rawSessions).forEach((sessionId) => {
    if (rawSessions[sessionId].info) {
      allSessions[sessionId] = rawSessions[sessionId].info;
    }
  });

  return allSessions;
});

const currentSession = computed(() => {
  if (!selectedSession.value) return null;
  const session = sessions.value[selectedSession.value];
  return session;
});

const sessionDataPoints = computed(() => {
  if (!selectedSession.value) return [];

  const rawData = dryerStore.currentSessionData;
  if (!rawData || Object.keys(rawData).length === 0) return [];

  return Object.values(rawData).sort((a: any, b: any) => {
    const parseTimestamp = (ts: string) => {
      const [date, time] = ts.split(" ");
      const [day, month, year] = date.split("/");
      return new Date(`${year}-${month}-${day} ${time}`).getTime();
    };

    return parseTimestamp(a.timestamp) - parseTimestamp(b.timestamp);
  });
});

const dataPointsCount = computed(() => {
  return sessionDataPoints.value.length;
});

const totalNotifications = computed(() => {
  return sessionDataPoints.value.reduce((total: number, data: any) => {
    return total + (data.notifications?.length || 0);
  }, 0);
});

const formatDateTime = (dateTime: string | undefined) => {
  if (!dateTime) return "-";

  const parts = dateTime.split(" ");
  if (parts.length === 2) {
    const [date, time] = parts;
    const [day, month, year] = date.split("/");
    const [hours, minutes] = time.split(":");

    if (window.innerWidth < 640) {
      return `${day}/${month} ${hours}:${minutes}`;
    }

    return dateTime;
  }

  return dateTime;
};

const loadSessionData = async () => {
  if (selectedSession.value) {
    isLoading.value = true;
    try {
      await dryerStore.fetchSessionData(selectedSession.value);
    } catch (error) {
      console.error("Error loading session data:", error);
    } finally {
      isLoading.value = false;
    }
  }
};

const getStatusClass = (status: string) => {
  const statusLower = status.toLowerCase();
  if (statusLower.includes("selesai") || statusLower.includes("completed")) {
    return "bg-green-100 text-green-800 dark:bg-green-900 dark:text-green-200";
  }
  if (statusLower.includes("berjalan") || statusLower.includes("running")) {
    return "bg-blue-100 text-blue-800 dark:bg-blue-900 dark:text-blue-200";
  }
  if (statusLower.includes("paused") || statusLower.includes("pause")) {
    return "bg-yellow-100 text-yellow-800 dark:bg-yellow-900 dark:text-yellow-200";
  }
  return "bg-gray-100 text-gray-800 dark:bg-gray-700 dark:text-gray-200";
};

const getNotificationColor = (notification: string): string => {
  const notifLower = notification.toLowerCase();
  if (notifLower.includes("error") || notifLower.includes("gagal")) {
    return "text-red-700 dark:text-red-300 font-medium";
  }
  if (
    notifLower.includes("warning") ||
    notifLower.includes("peringatan") ||
    notifLower.includes("pintu") ||
    notifLower.includes("terbuka")
  ) {
    return "text-yellow-700 dark:text-yellow-300 font-medium";
  }
  if (notifLower.includes("proteksi") || notifLower.includes("suhu tinggi")) {
    return "text-red-700 dark:text-red-300 font-medium";
  }
  if (
    notifLower.includes("selesai") ||
    notifLower.includes("completed") ||
    notifLower.includes("success")
  ) {
    return "text-green-700 dark:text-green-300 font-medium";
  }
  return "text-gray-700 dark:text-gray-300";
};

const downloadPDF = () => {
  if (!currentSession.value || dataPointsCount.value === 0) return;

  const doc = new jsPDF("landscape");

  doc.setFontSize(20);
  doc.setFont("helvetica", "bold");
  doc.text("LAPORAN PENGERINGAN CABAI", 148, 15, { align: "center" });

  doc.setFontSize(11);
  doc.setFont("helvetica", "normal");

  const infoY = 25;
  doc.text(`Session ID: ${currentSession.value.session_id}`, 14, infoY);
  doc.text(`Waktu Mulai: ${currentSession.value.start_time}`, 14, infoY + 6);
  doc.text(
    `Waktu Selesai: ${currentSession.value.end_time || "Sedang berjalan"}`,
    14,
    infoY + 12
  );
  doc.text(`Status: ${currentSession.value.status}`, 14, infoY + 18);
  doc.text(
    `Berat Awal: ${currentSession.value.initial_weight?.toFixed(2)} gram`,
    14,
    infoY + 24
  );

  if (currentSession.value.final_weight) {
    doc.text(
      `Berat Akhir: ${currentSession.value.final_weight.toFixed(2)} gram`,
      14,
      infoY + 30
    );
  }
  if (currentSession.value.final_moisture) {
    doc.text(
      `Kadar Air Akhir: ${currentSession.value.final_moisture.toFixed(2)}%`,
      14,
      infoY + 36
    );
  }

  const avgTemp =
    sessionDataPoints.value.reduce(
      (sum: number, d: any) => sum + d.temperature,
      0
    ) / dataPointsCount.value;
  const avgHumidity =
    sessionDataPoints.value.reduce(
      (sum: number, d: any) => sum + d.humidity,
      0
    ) / dataPointsCount.value;

  doc.setFont("helvetica", "bold");
  doc.text("Rata-rata:", 14, infoY + 48);
  doc.setFont("helvetica", "normal");
  doc.text(
    `Suhu: ${avgTemp.toFixed(1)}°C | Kelembapan: ${avgHumidity.toFixed(1)}%`,
    14,
    infoY + 54
  );

  doc.text(`Total Notifikasi: ${totalNotifications.value}`, 14, infoY + 60);

  const tableData = sessionDataPoints.value.map((data: any, index: number) => {
    const notifications =
      data.notifications && data.notifications.length > 0
        ? data.notifications.join("; ")
        : "-";

    return [
      index + 1,
      data.timestamp,
      data.temperature.toFixed(1),
      data.humidity.toFixed(1),
      data.moisture.toFixed(1),
      data.weight.toFixed(1),
      data.status,
      data.heater,
      data.fan,
      data.exhaust,
      notifications,
    ];
  });

  autoTable(doc, {
    startY: infoY + 68,
    head: [
      [
        "No",
        "Waktu",
        "Suhu",
        "RH",
        "KA",
        "Berat",
        "Status",
        "Heater",
        "Fan",
        "Exhaust",
        "Keterangan",
      ],
    ],
    body: tableData,
    theme: "grid",
    styles: {
      fontSize: 7,
      cellPadding: 2,
    },
    headStyles: {
      fillColor: [220, 38, 38],
      textColor: 255,
      fontStyle: "bold",
    },
    alternateRowStyles: {
      fillColor: [245, 245, 245],
    },
    columnStyles: {
      0: { cellWidth: 10 },
      1: { cellWidth: 30 },
      2: { cellWidth: 15 },
      3: { cellWidth: 15 },
      4: { cellWidth: 15 },
      5: { cellWidth: 18 },
      6: { cellWidth: 20 },
      7: { cellWidth: 18 },
      8: { cellWidth: 18 },
      9: { cellWidth: 20 },
      10: { cellWidth: "auto" },
    },
  });

  const pageCount = (doc as any).internal.getNumberOfPages();
  doc.setFontSize(8);
  doc.setTextColor(128);
  for (let i = 1; i <= pageCount; i++) {
    doc.setPage(i);
    doc.text(
      `Halaman ${i} dari ${pageCount}`,
      148,
      doc.internal.pageSize.height - 10,
      { align: "center" }
    );
    doc.text(
      `Generated: ${new Date().toLocaleString("id-ID")}`,
      14,
      doc.internal.pageSize.height - 10
    );
  }

  const filename = `rekap-pengeringan-${selectedSession.value}.pdf`;
  doc.save(filename);
};

const deleteSession = async () => {
  if (!selectedSession.value) return;

  const confirmed = confirm(
    `Apakah Anda yakin ingin menghapus data session ini?\n\n` +
      `Session ID: ${currentSession.value?.session_id}\n` +
      `Total data: ${dataPointsCount.value} records\n` +
      `Total notifikasi: ${totalNotifications.value}\n\n` +
      `Data yang dihapus tidak dapat dikembalikan!`
  );

  if (!confirmed) return;

  try {
    await remove(dbRef(database, `sessions/${selectedSession.value}`));
    alert("Data berhasil dihapus!");
    selectedSession.value = "";
  } catch (error) {
    console.error("Error deleting session:", error);
    alert("Gagal menghapus data. Silakan coba lagi.");
  }
};
</script>
