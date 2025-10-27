<template>
  <div class="flex min-h-screen bg-gray-100 dark:bg-gray-900">
    <Sidebar />

    <div class="flex-1">
      <Header />

      <main class="p-8">
        <div class="max-w-7xl mx-auto">
          <div
            class="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-6 transition-colors"
          >
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
                <option value="">Semua data</option>
                <option v-for="(session, id) in sessions" :key="id" :value="id">
                  {{ session.session_id }} - {{ session.start_time }}
                </option>
              </select>
            </div>

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
              <p class="text-lg mb-2">data masih kosong</p>
            </div>

            <div v-else-if="isLoading" class="text-center py-12">
              <div
                class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary"
              ></div>
              <p class="mt-4 text-gray-600 dark:text-gray-400">
                Memuat data...
              </p>
            </div>

            <div v-else>
              <div class="mb-6 grid grid-cols-1 md:grid-cols-4 gap-4 text-sm">
                <div class="bg-gray-50 dark:bg-gray-700 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-400">Session ID</p>
                  <p class="font-semibold text-gray-800 dark:text-white">
                    {{ currentSession?.session_id }}
                  </p>
                </div>
                <div class="bg-gray-50 dark:bg-gray-700 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-400">Waktu Mulai</p>
                  <p class="font-semibold text-gray-800 dark:text-white">
                    {{ currentSession?.start_time }}
                  </p>
                </div>
                <div class="bg-gray-50 dark:bg-gray-700 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-400">Waktu Selesai</p>
                  <p class="font-semibold text-gray-800 dark:text-white">
                    {{ currentSession?.end_time || "Sedang berjalan" }}
                  </p>
                </div>
                <div class="bg-gray-50 dark:bg-gray-700 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-400">Status</p>
                  <p
                    class="font-semibold capitalize text-gray-800 dark:text-white"
                  >
                    {{ currentSession?.status }}
                  </p>
                </div>
              </div>

              <div class="mb-6 grid grid-cols-1 md:grid-cols-3 gap-4 text-sm">
                <div class="bg-blue-50 dark:bg-blue-900 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-300">Berat Awal</p>
                  <p
                    class="text-2xl font-bold text-blue-600 dark:text-blue-300"
                  >
                    {{ currentSession?.initial_weight }} g
                  </p>
                </div>
                <div class="bg-green-50 dark:bg-green-900 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-300">Berat Akhir</p>
                  <p
                    class="text-2xl font-bold text-green-600 dark:text-green-300"
                  >
                    {{ currentSession?.final_weight || "-" }} g
                  </p>
                </div>
                <div class="bg-purple-50 dark:bg-purple-900 p-4 rounded-lg">
                  <p class="text-gray-600 dark:text-gray-300">
                    Kadar Air Akhir
                  </p>
                  <p
                    class="text-2xl font-bold text-purple-600 dark:text-purple-300"
                  >
                    {{ currentSession?.final_moisture || "-" }} %
                  </p>
                </div>
              </div>

              <div
                v-if="dataPointsCount > 0"
                class="mb-4 text-sm text-gray-600 dark:text-gray-400"
              >
                Total data points:
                <span class="font-semibold">{{ dataPointsCount }}</span>
              </div>

              <div
                class="overflow-x-auto rounded-lg border border-gray-200 dark:border-gray-700"
              >
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
                    </tr>
                  </thead>
                  <tbody
                    class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700"
                  >
                    <tr v-if="dataPointsCount === 0">
                      <td
                        colspan="10"
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
                        class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
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
                          class="px-2 py-1 rounded-full text-xs font-semibold"
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
                    </tr>
                  </tbody>
                </table>
              </div>

              <div class="mt-6 flex flex-col sm:flex-row gap-4">
                <button
                  @click="downloadPDF"
                  :disabled="dataPointsCount === 0"
                  class="flex-1 sm:flex-none px-6 py-3 bg-blue-500 text-white rounded-lg font-semibold hover:bg-blue-600 transition disabled:opacity-50 disabled:cursor-not-allowed flex items-center justify-center space-x-2"
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
                  class="flex-1 sm:flex-none px-6 py-3 bg-red-500 text-white rounded-lg font-semibold hover:bg-red-600 transition flex items-center justify-center space-x-2"
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

              <div class="mt-4 p-4 bg-yellow-50 dark:bg-yellow-900 rounded-lg">
                <p class="text-xs text-gray-700 dark:text-gray-300">
                  <strong>Catatan:</strong> Data rekap akan otomatis tersimpan
                  setiap 15 detik selama proses pengeringan berlangsung. Export
                  PDF akan menyertakan semua data dalam tabel.
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
import type { SessionDataPoint } from "@/types";

const dryerStore = useDryerStore();
const selectedSession = ref("");
const isLoading = ref(false);

const sessions = computed(() => dryerStore.sessions);

const currentSession = computed(() => {
  if (!selectedSession.value) return null;
  return sessions.value[selectedSession.value];
});

const sessionDataPoints = computed(() => {
  const data = dryerStore.currentSessionData;
  return Object.values(data).sort((a: any, b: any) => {
    return new Date(a.timestamp).getTime() - new Date(b.timestamp).getTime();
  });
});

const dataPointsCount = computed(() => {
  return sessionDataPoints.value.length;
});

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

const downloadPDF = () => {
  if (!currentSession.value || dataPointsCount.value === 0) return;

  const doc = new jsPDF();

  // Header
  doc.setFontSize(20);
  doc.setFont("helvetica", "bold");
  doc.text("LAPORAN PENGERINGAN CABAI", 105, 20, { align: "center" });

  // Session Info
  doc.setFontSize(11);
  doc.setFont("helvetica", "normal");

  const infoY = 35;
  doc.text(`Session ID: ${currentSession.value.session_id}`, 14, infoY);
  doc.text(`Waktu Mulai: ${currentSession.value.start_time}`, 14, infoY + 7);
  doc.text(
    `Waktu Selesai: ${currentSession.value.end_time || "Sedang berjalan"}`,
    14,
    infoY + 14
  );
  doc.text(`Status: ${currentSession.value.status}`, 14, infoY + 21);
  doc.text(
    `Berat Awal: ${currentSession.value.initial_weight} gram`,
    14,
    infoY + 28
  );

  if (currentSession.value.final_weight) {
    doc.text(
      `Berat Akhir: ${currentSession.value.final_weight} gram`,
      14,
      infoY + 35
    );
  }
  if (currentSession.value.final_moisture) {
    doc.text(
      `Kadar Air Akhir: ${currentSession.value.final_moisture}%`,
      14,
      infoY + 42
    );
  }

  // Summary statistics
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
  doc.text("Rata-rata:", 14, infoY + 56);
  doc.setFont("helvetica", "normal");
  doc.text(
    `Suhu: ${avgTemp.toFixed(1)}°C | Kelembapan: ${avgHumidity.toFixed(1)}%`,
    14,
    infoY + 63
  );

  // Table data
  const tableData = sessionDataPoints.value.map((data: any, index: number) => [
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
  ]);

  autoTable(doc, {
    startY: infoY + 70,
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
      1: { cellWidth: 35 },
      2: { cellWidth: 15 },
      3: { cellWidth: 15 },
      4: { cellWidth: 15 },
      5: { cellWidth: 18 },
      6: { cellWidth: 20 },
      7: { cellWidth: 18 },
      8: { cellWidth: 20 },
      9: { cellWidth: 20 },
    },
  });

  // Footer
  const pageCount = (doc as any).internal.getNumberOfPages();
  doc.setFontSize(8);
  doc.setTextColor(128);
  for (let i = 1; i <= pageCount; i++) {
    doc.setPage(i);
    doc.text(
      `Halaman ${i} dari ${pageCount}`,
      105,
      doc.internal.pageSize.height - 10,
      { align: "center" }
    );
    doc.text(
      `Generated: ${new Date().toLocaleString("id-ID")}`,
      14,
      doc.internal.pageSize.height - 10
    );
  }

  // Save PDF
  const filename = `rekap-pengeringan-${selectedSession.value}.pdf`;
  doc.save(filename);
};

const deleteSession = async () => {
  if (!selectedSession.value) return;

  const confirmed = confirm(
    `Apakah Anda yakin ingin menghapus data session ini?\n\n` +
      `Session ID: ${currentSession.value?.session_id}\n` +
      `Total data: ${dataPointsCount.value} records\n\n` +
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
