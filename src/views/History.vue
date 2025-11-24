<template>
  <FloatingQnA />
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
            <!-- Title & Sort Controls -->
            <div class="flex items-center justify-between mb-6">
              <h2 class="text-xl font-bold text-gray-800 dark:text-white">
                Rekap Pengeringan
              </h2>
              <div class="flex items-center space-x-2">
                <label class="text-sm text-gray-600 dark:text-gray-400">
                  Urutkan:
                </label>
                <select
                  v-model="sortOrder"
                  class="px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent bg-white dark:bg-gray-700 text-gray-800 dark:text-white text-sm"
                >
                  <option value="newest">Terbaru</option>
                  <option value="oldest">Terlama</option>
                </select>
              </div>
            </div>

            <!-- Main Sessions Table -->
            <div
              v-if="currentView === 'list'"
              class="overflow-hidden rounded-lg border border-gray-200 dark:border-gray-700"
            >
              <!-- Desktop Table -->
              <div class="hidden md:block overflow-x-auto">
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
                        SAP
                      </th>
                      <th
                        class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                      >
                        Date and Time
                      </th>
                      <th
                        class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase tracking-wider"
                      >
                        Action
                      </th>
                    </tr>
                  </thead>
                  <tbody
                    class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700"
                  >
                    <tr v-if="sortedSessions.length === 0">
                      <td
                        colspan="4"
                        class="px-4 py-8 text-center text-gray-500 dark:text-gray-400"
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
                        <p class="text-base">Tidak ada data pengeringan</p>
                      </td>
                    </tr>
                    <tr
                      v-for="(session, index) in sortedSessions"
                      :key="session.id"
                      @click="viewRecap30Min(session.id)"
                      class="hover:bg-gray-50 dark:hover:bg-gray-700 transition cursor-pointer"
                    >
                      <td
                        class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                      >
                        {{ index + 1 }}
                      </td>
                      <td
                        class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                      >
                        {{ session.session_id }}
                      </td>
                      <td
                        class="px-4 py-3 text-sm text-gray-800 dark:text-gray-300"
                      >
                        {{ getSessionName(session) }}
                      </td>
                      <td class="px-4 py-3 text-sm" @click.stop>
                        <div class="flex items-center space-x-2">
                          <button
                            @click="viewDetailData(session.id)"
                            class="px-3 py-1.5 bg-yellow-500 text-white rounded-lg hover:bg-yellow-600 transition text-xs font-semibold"
                          >
                            Detail
                          </button>
                          <button
                            @click="downloadRecapPDF(session.id)"
                            class="px-3 py-1.5 bg-blue-500 text-white rounded-lg hover:bg-blue-600 transition text-xs font-semibold"
                          >
                            Unduh
                          </button>
                          <button
                            @click="deleteSession(session.id)"
                            class="px-3 py-1.5 bg-red-500 text-white rounded-lg hover:bg-red-600 transition text-xs font-semibold"
                          >
                            Hapus
                          </button>
                        </div>
                      </td>
                    </tr>
                  </tbody>
                </table>
              </div>

              <!-- Mobile Cards -->
              <div
                class="md:hidden divide-y divide-gray-200 dark:divide-gray-700"
              >
                <div
                  v-if="sortedSessions.length === 0"
                  class="p-6 text-center text-gray-500 dark:text-gray-400"
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
                  <p class="text-base">Tidak ada data pengeringan</p>
                </div>

                <div
                  v-for="(session, index) in sortedSessions"
                  :key="session.id"
                  @click="viewRecap30Min(session.id)"
                  class="p-4 bg-white dark:bg-gray-800 hover:bg-gray-50 dark:hover:bg-gray-700 transition cursor-pointer"
                >
                  <div class="flex items-center justify-between mb-3">
                    <span
                      class="text-xs font-semibold text-gray-500 dark:text-gray-400"
                    >
                      #{{ index + 1 }}
                    </span>
                    <span class="text-xs text-gray-500 dark:text-gray-400">
                      {{ session.session_id }}
                    </span>
                  </div>
                  <p
                    class="text-sm font-semibold text-gray-800 dark:text-white mb-3"
                  >
                    {{ getSessionName(session) }}
                  </p>
                  <div class="flex items-center space-x-2" @click.stop>
                    <button
                      @click="viewDetailData(session.id)"
                      class="flex-1 px-3 py-2 bg-yellow-500 text-white rounded-lg hover:bg-yellow-600 transition text-xs font-semibold"
                    >
                      Detail
                    </button>
                    <button
                      @click="downloadRecapPDF(session.id)"
                      class="flex-1 px-3 py-2 bg-blue-500 text-white rounded-lg hover:bg-blue-600 transition text-xs font-semibold"
                    >
                      Unduh
                    </button>
                    <button
                      @click="deleteSession(session.id)"
                      class="flex-1 px-3 py-2 bg-red-500 text-white rounded-lg hover:bg-red-600 transition text-xs font-semibold"
                    >
                      Hapus
                    </button>
                  </div>
                </div>
              </div>
            </div>

            <!-- Recap 30 Min View -->
            <div v-else-if="currentView === 'recap30'" class="space-y-6">
              <div class="flex items-center justify-between">
                <button
                  @click="backToList"
                  class="flex items-center space-x-2 text-primary hover:underline"
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
                      d="M10 19l-7-7m0 0l7-7m-7 7h18"
                    />
                  </svg>
                  <span class="font-semibold">Kembali</span>
                </button>
              </div>

              <!-- Session Info Cards - Responsive Grid -->
              <div
                class="grid grid-cols-2 lg:grid-cols-4 gap-3 sm:gap-4 text-xs sm:text-sm"
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
                class="grid grid-cols-1 sm:grid-cols-3 gap-3 sm:gap-4 text-xs sm:text-sm"
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
              <div class="flex flex-wrap gap-4 text-xs sm:text-sm">
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

              <div v-if="isLoading" class="text-center py-12">
                <div
                  class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary"
                ></div>
                <p class="mt-4 text-gray-600 dark:text-gray-400">
                  Memuat data rekap...
                </p>
              </div>

              <div v-else>
                <div
                  class="overflow-hidden rounded-lg border border-gray-200 dark:border-gray-700"
                >
                  <div class="overflow-x-auto">
                    <table class="w-full">
                      <thead class="bg-gray-50 dark:bg-gray-700">
                        <tr>
                          <th
                            class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase"
                          >
                            Interval
                          </th>
                          <th
                            class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase"
                          >
                            Rata Suhu
                          </th>
                          <th
                            class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase"
                          >
                            Rata Kelembapan
                          </th>
                          <th
                            class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase"
                          >
                            Rata Kadar Air
                          </th>
                          <th
                            class="px-4 py-3 text-left text-xs font-medium text-gray-500 dark:text-gray-300 uppercase"
                          >
                            Error
                          </th>
                        </tr>
                      </thead>
                      <tbody
                        class="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700"
                      >
                        <tr v-if="recap30MinData.length === 0">
                          <td
                            colspan="5"
                            class="px-4 py-8 text-center text-gray-500"
                          >
                            Tidak ada data
                          </td>
                        </tr>
                        <tr
                          v-for="(item, idx) in recap30MinData"
                          :key="idx"
                          class="hover:bg-gray-50 dark:hover:bg-gray-700"
                        >
                          <td class="px-4 py-3 text-sm">
                            {{ item.timeRange }}
                          </td>
                          <td class="px-4 py-3 text-sm">
                            {{ item.avgTemp.toFixed(1) }}°C
                          </td>
                          <td class="px-4 py-3 text-sm">
                            {{ item.avgHumidity.toFixed(1) }}%
                          </td>
                          <td class="px-4 py-3 text-sm">
                            {{ item.avgMoisture.toFixed(1) }}%
                          </td>
                          <td class="px-4 py-3 text-sm">
                            <span
                              v-if="item.errors.length === 0"
                              class="text-gray-400"
                              >-</span
                            >
                            <div v-else class="text-xs text-red-600">
                              <div v-for="(err, i) in item.errors" :key="i">
                                {{ err }}
                              </div>
                            </div>
                          </td>
                        </tr>
                      </tbody>
                    </table>
                  </div>
                </div>

                <div class="flex gap-4 mt-6">
                  <button
                    @click="downloadRecap30PDF"
                    class="flex-1 px-6 py-3 bg-blue-500 text-white rounded-lg hover:bg-blue-600 flex items-center justify-center gap-2"
                  >
                    <span>Download PDF</span>
                  </button>
                  <button
                    @click="viewDetailData(selectedSessionId)"
                    class="flex-1 px-3 py-2 bg-yellow-500 text-white rounded-lg hover:bg-yellow-600 transition text-xs font-semibold"
                  >
                    Detail
                  </button>
                </div>
              </div>
            </div>

            <!-- Detail View -->
            <div v-else-if="currentView === 'detail'" class="space-y-6">
              <div class="flex items-center justify-between">
                <button
                  @click="backToList"
                  class="flex items-center space-x-2 text-primary hover:underline"
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
                      d="M10 19l-7-7m0 0l7-7m-7 7h18"
                    />
                  </svg>
                  <span class="font-semibold">Kembali</span>
                </button>
                <h3 class="text-lg font-bold text-gray-800 dark:text-white">
                  Detail Data
                </h3>
              </div>

              <!-- Session Info Cards - Responsive Grid -->
              <div
                class="grid grid-cols-2 lg:grid-cols-4 gap-3 sm:gap-4 text-xs sm:text-sm"
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
                class="grid grid-cols-1 sm:grid-cols-3 gap-3 sm:gap-4 text-xs sm:text-sm"
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
              <div class="flex flex-wrap gap-4 text-xs sm:text-sm">
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

              <div v-if="isLoading" class="text-center py-12">
                <div
                  class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary"
                ></div>
                <p class="mt-4 text-gray-600 dark:text-gray-400">
                  Memuat data...
                </p>
              </div>

              <div v-else>
                <div
                  class="overflow-hidden rounded-lg border border-gray-200 dark:border-gray-700"
                >
                  <div class="overflow-x-auto">
                    <table class="w-full text-sm">
                      <thead class="bg-gray-50 dark:bg-gray-700">
                        <tr>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            No
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Waktu
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Suhu
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Kelembapan
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Kadar Air
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Berat
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Status
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Heater
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Fan
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Exhaust
                          </th>
                          <th class="px-4 py-3 text-left text-xs font-medium">
                            Keterangan
                          </th>
                        </tr>
                      </thead>
                      <tbody class="bg-white dark:bg-gray-800 divide-y">
                        <tr v-if="sessionDataPoints.length === 0">
                          <td colspan="11" class="px-4 py-8 text-center">
                            Tidak ada data
                          </td>
                        </tr>
                        <tr
                          v-for="(d, i) in sessionDataPoints"
                          :key="i"
                          class="hover:bg-gray-50 dark:hover:bg-gray-700"
                        >
                          <td class="px-4 py-3">{{ i + 1 }}</td>
                          <td class="px-4 py-3 whitespace-nowrap">
                            {{ d.timestamp }}
                          </td>
                          <td class="px-4 py-3">
                            {{ d.temperature.toFixed(1) }}
                          </td>
                          <td class="px-4 py-3">{{ d.humidity.toFixed(1) }}</td>
                          <td class="px-4 py-3">{{ d.moisture.toFixed(1) }}</td>
                          <td class="px-4 py-3">{{ d.weight.toFixed(1) }}</td>
                          <td class="px-4 py-3">{{ d.status }}</td>
                          <td class="px-4 py-3">{{ d.heater }}</td>
                          <td class="px-4 py-3">{{ d.fan }}</td>
                          <td class="px-4 py-3">{{ d.exhaust }}</td>
                          <td class="px-4 py-3">
                            <span
                              v-if="
                                !d.notifications || d.notifications.length === 0
                              "
                              class="text-gray-400"
                              >-</span
                            >
                            <div v-else class="text-xs">
                              <div v-for="(n, ni) in d.notifications" :key="ni">
                                {{ n }}
                              </div>
                            </div>
                          </td>
                        </tr>
                      </tbody>
                    </table>
                  </div>
                </div>

                <div class="flex gap-4 mt-6">
                  <button
                    @click="downloadDetailPDF"
                    class="flex-1 px-6 py-3 bg-blue-500 text-white rounded-lg hover:bg-blue-600"
                  >
                    Download PDF
                  </button>
                  <button
                    @click="downloadRawJSON"
                    class="flex-1 px-6 py-3 bg-green-500 text-white rounded-lg hover:bg-green-600"
                  >
                    Download RAW
                  </button>
                </div>
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
import FloatingQnA from "@/components/FloatingQnA.vue";

const dryerStore = useDryerStore();
const isSidebarCollapsed = ref(true);
const currentView = ref<"list" | "recap30" | "detail">("list");
const selectedSessionId = ref("");
const isLoading = ref(false);
const sortOrder = ref<"newest" | "oldest">("newest");

const toggleSidebar = () => {
  isSidebarCollapsed.value = !isSidebarCollapsed.value;
};

const sessions = computed(() => {
  const all: Array<{ id: string; [key: string]: any }> = [];
  const raw = dryerStore.sessions;
  Object.keys(raw).forEach((id) => {
    if (raw[id].info) all.push({ id, ...raw[id].info });
  });
  return all;
});

const sortedSessions = computed(() => {
  const sorted = [...sessions.value];
  sorted.sort((a, b) => {
    const parse = (ts: string) => {
      const [date, time] = ts.split(" ");
      const [d, m, y] = date.split("/");
      return new Date(`${y}-${m}-${d} ${time}`).getTime();
    };
    const ta = parse(a.start_time);
    const tb = parse(b.start_time);
    return sortOrder.value === "newest" ? tb - ta : ta - tb;
  });
  return sorted;
});

const currentSession = computed(() => {
  if (!selectedSessionId.value) return null;
  return sessions.value.find((s) => s.id === selectedSessionId.value);
});

const sessionDataPoints = computed(() => {
  if (!selectedSessionId.value) return [];
  const raw = dryerStore.currentSessionData;
  if (!raw || Object.keys(raw).length === 0) return [];
  return Object.values(raw).sort((a: any, b: any) => {
    const parse = (ts: string) => {
      const [date, time] = ts.split(" ");
      const [d, m, y] = date.split("/");
      return new Date(`${y}-${m}-${d} ${time}`).getTime();
    };
    return parse(a.timestamp) - parse(b.timestamp);
  });
});

const recap30MinData = computed(() => {
  if (sessionDataPoints.value.length === 0) return [];
  const intervals: any[] = [];
  const data = sessionDataPoints.value;
  const parse = (ts: string) => {
    const [date, time] = ts.split(" ");
    const [d, m, y] = date.split("/");
    const [h, min, s] = time.split(":");
    return new Date(+y, +m - 1, +d, +h, +min, +s);
  };
  const start = parse(data[0].timestamp);
  const end = parse(data[data.length - 1].timestamp);
  const dur = end.getTime() - start.getTime();
  const count = Math.ceil(dur / (30 * 60 * 1000));
  for (let i = 0; i < count; i++) {
    const is = new Date(start.getTime() + i * 30 * 60 * 1000);
    const ie = new Date(start.getTime() + (i + 1) * 30 * 60 * 1000);
    const id = data.filter((d: any) => {
      const t = parse(d.timestamp);
      return t >= is && t < ie;
    });
    if (id.length === 0) continue;
    const avgT =
      id.reduce((s: number, d: any) => s + d.temperature, 0) / id.length;
    const avgH =
      id.reduce((s: number, d: any) => s + d.humidity, 0) / id.length;
    const avgM =
      id.reduce((s: number, d: any) => s + d.moisture, 0) / id.length;
    const errs: string[] = [];
    id.forEach((d: any) => {
      if (d.notifications)
        d.notifications.forEach((n: string) => {
          if (!errs.includes(n)) errs.push(n);
        });
    });
    const fmt = (d: Date) => {
      const h = d.getHours().toString().padStart(2, "0");
      const m = d.getMinutes().toString().padStart(2, "0");
      return `${h}:${m}`;
    };
    intervals.push({
      timeRange: `${fmt(is)} - ${fmt(ie)}`,
      avgTemp: avgT,
      avgHumidity: avgH,
      avgMoisture: avgM,
      errors: errs,
      dataCount: id.length,
    });
  }
  return intervals;
});

const dataPointsCount = computed(() => sessionDataPoints.value.length);

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

const getSessionName = (s: any) => `${s.start_time} - ${s.status || "Unknown"}`;

const viewRecap30Min = async (id: string) => {
  selectedSessionId.value = id;
  isLoading.value = true;
  currentView.value = "recap30";
  try {
    await dryerStore.fetchSessionData(id);
  } finally {
    isLoading.value = false;
  }
};

const viewDetailData = async (id: string) => {
  selectedSessionId.value = id;
  isLoading.value = true;
  currentView.value = "detail";
  try {
    await dryerStore.fetchSessionData(id);
  } finally {
    isLoading.value = false;
  }
};

const backToList = () => {
  currentView.value = "list";
  selectedSessionId.value = "";
};

const downloadRecapPDF = async (id: string) => {
  await dryerStore.fetchSessionData(id);
  const temp = selectedSessionId.value;
  selectedSessionId.value = id;
  await new Promise((r) => setTimeout(r, 100));
  downloadRecap30PDF();
  selectedSessionId.value = temp;
};

const downloadRecap30PDF = () => {
  if (!currentSession.value || recap30MinData.value.length === 0) return;
  const doc = new jsPDF("landscape");
  doc.setFontSize(20);
  doc.text("REKAP 30 MENIT", 148, 15, { align: "center" });
  const td = recap30MinData.value.map((v: any, i: number) => [
    i + 1,
    v.timeRange,
    v.avgTemp.toFixed(1),
    v.avgHumidity.toFixed(1),
    v.avgMoisture.toFixed(1),
    v.dataCount,
    v.errors.length > 0 ? v.errors.join("; ") : "-",
  ]);
  autoTable(doc, {
    startY: 25,
    head: [
      ["No", "Interval", "Suhu", "Kelembapan", "Kdar Air", "Data", "Error"],
    ],
    body: td,
  });
  doc.save(`rekap-30min-${selectedSessionId.value}.pdf`);
};

const downloadDetailPDF = () => {
  if (!currentSession.value || sessionDataPoints.value.length === 0) return;
  const doc = new jsPDF("landscape");
  doc.setFontSize(20);
  doc.text("DETAIL DATA", 148, 15, { align: "center" });
  const td = sessionDataPoints.value.map((d: any, i: number) => [
    i + 1,
    d.timestamp,
    d.temperature.toFixed(1),
    d.humidity.toFixed(1),
    d.moisture.toFixed(1),
    d.weight.toFixed(1),
    d.status,
    d.heater,
    d.fan,
    d.exhaust,
    d.notifications?.join("; ") || "-",
  ]);
  autoTable(doc, {
    startY: 25,
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
    body: td,
    styles: { fontSize: 7 },
  });
  doc.save(`detail-${selectedSessionId.value}.pdf`);
};

const downloadRawJSON = () => {
  if (!currentSession.value || sessionDataPoints.value.length === 0) return;
  const raw = {
    session: currentSession.value,
    data: sessionDataPoints.value,
    total: sessionDataPoints.value.length,
    exported: new Date().toISOString(),
  };
  const str = JSON.stringify(raw, null, 2);
  const blob = new Blob([str], { type: "application/json" });
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a");
  link.href = url;
  link.download = `raw-${selectedSessionId.value}.json`;
  link.click();
  URL.revokeObjectURL(url);
};

const deleteSession = async (id: string) => {
  const s = sessions.value.find((x) => x.id === id);
  if (!s) return;
  const ok = confirm(
    `Hapus session ini?\n\nSession: ${s.session_id}\nWaktu: ${s.start_time}\n\nData tidak dapat dikembalikan!`
  );
  if (!ok) return;
  try {
    await remove(dbRef(database, `sessions/${id}`));
    alert("Data berhasil dihapus!");
    if (selectedSessionId.value === id) backToList();
  } catch (e) {
    console.error(e);
    alert("Gagal menghapus data!");
  }
};

const deleteCurrentSession = async () => {
  if (!selectedSessionId.value) return;
  await deleteSession(selectedSessionId.value);
};
</script>
