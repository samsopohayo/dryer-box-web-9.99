<!-- components/FloatingQnA.vue -->
<template>
  <!-- Floating Button -->
  <div class="fixed bottom-6 right-6 z-50">
    <!-- Main FAB Button -->
    <button
      @click="isMainVisible = !isMainVisible"
      class="w-15 h-15 rounded-full shadow-2xl flex items-center justify-center hover:scale-110 transition-all duration-300"
      v-show="isMainVisible || !isMainVisible"
    >
      <img src="/icons/qna-icon.png" alt="QnA" class="w-10 h-10" />
    </button>

    <!-- Menu Utama -->
    <transition name="fade-scale">
      <div
        v-if="isMainVisible"
        class="absolute bottom-11 right-0 w-64 z-60 bg-white dark:bg-gray-800 rounded-2xl shadow-2xl overflow-hidden"
      >
        <button
          @click="openTanyaAdmin"
          class="w-full px-6 py-4 text-left hover:bg-gray-100 dark:hover:bg-gray-700 transition flex items-center space-x-3"
        >
          <svg
            class="w-6 h-6 text-green-600"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M8 12h.01M12 12h.01M16 12h.01M21 12c0 4.418-4.03 8-9 8a9.863 9.863 0 01-4.255-.949L3 20l1.395-3.72C3.512 15.042 3 13.574 3 12c0-4.418 4.03-8 9-8s9 3.582 9 8z"
            />
          </svg>
          <span class="font-medium">Tanya Admin</span>
        </button>
        <button
          @click="showTutorialMenu = true"
          class="w-full px-6 py-4 text-left hover:bg-gray-100 dark:hover:bg-gray-700 transition flex items-center space-x-3"
        >
          <svg
            class="w-6 h-6 text-blue-600"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M12 6.253v13m0-13C10.832 5.477 9.246 5 7.5 5S4.168 5.477 3 6.253v13C4.168 18.477 5.754 18 7.5 18s3.332.477 4.5 1.253m0-13C13.168 5.477 14.754 5 16.5 5c1.747 0 3.332.477 4.5 1.253v13C19.832 18.477 18.247 18 16.5 18c-1.746 0-3.332.477-4.5 1.253"
            />
          </svg>
          <span class="font-medium">Lihat Tutorial</span>
        </button>
      </div>
    </transition>
  </div>

  <!-- Panel Tanya Admin -->
  <transition name="slide">
    <div
      v-if="showTanyaAdmin"
      class="fixed inset-0 bg-black bg-opacity-50 z-50 backdrop-blur-sm flex items-end justify-end p-4"
      @click.self="showTanyaAdmin = false"
    >
      <div
        class="w-full max-w-md bg-white dark:bg-gray-800 rounded-2xl shadow-2xl p-6 animate-slide-up"
      >
        <div class="flex justify-between items-center mb-4">
          <h3 class="text-xl font-bold">Tanya Admin</h3>
          <button
            @click="showTanyaAdmin = false"
            class="text-gray-500 hover:text-gray-700"
          >
            <svg
              class="w-6 h-6"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M6 18L18 6M6 6l12 12"
              />
            </svg>
          </button>
        </div>
        <textarea
          v-model="pesan"
          placeholder="Tulis pertanyaan kamu di sini..."
          class="w-full h-40 p-4 border border-gray-300 dark:border-gray-600 rounded-lg focus:ring-2 focus:ring-indigo-500 focus:border-transparent dark:bg-gray-700"
        ></textarea>
        <div class="flex justify-between mt-4">
          <button
            @click="pesan = ''"
            class="px-6 py-3 bg-gray-300 dark:bg-gray-600 rounded-lg hover:bg-gray-400 transition"
          >
            Clear
          </button>
          <button
            @click="kirimWA"
            class="px-6 py-3 bg-green-600 text-white rounded-lg hover:bg-green-700 transition flex items-center space-x-2"
          >
            <span>Kirim ke WhatsApp</span>
          </button>
        </div>
      </div>
    </div>
  </transition>

  <!-- Menu Tutorial Level 1 -->
  <transition name="slide">
    <div
      v-if="showTutorialMenu && !selectedTutorial"
      class="fixed inset-0 bg-black bg-opacity-50 z-50 backdrop-blur-sm flex items-end justify-end p-4"
      @click.self="showTutorialMenu = false"
    >
      <div
        class="w-full max-w-md bg-white dark:bg-gray-800 rounded-2xl shadow-2xl p-6"
      >
        <div class="flex justify-between items-center mb-6">
          <h3 class="text-xl font-bold">Pilih Tutorial</h3>
          <button @click="showTutorialMenu = false" class="text-gray-500">
            <svg
              class="w-6 h-6"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M6 18L18 6M6 6l12 12"
              />
            </svg>
          </button>
        </div>
        <div class="space-y-3">
          <button
            @click="selectedTutorial = 'alat'"
            class="w-full text-left p-4 rounded-lg hover:bg-gray-100 dark:hover:bg-gray-700 transition"
          >
            Tutorial Alat
          </button>
          <button
            @click="selectedTutorial = 'dashboard'"
            class="w-full text-left p-4 rounded-lg hover:bg-gray-100 dark:hover:bg-gray-700 transition"
          >
            Tutorial Dashboard
          </button>
          <button
            @click="selectedTutorial = 'control'"
            class="w-full text-left p-4 rounded-lg hover:bg-gray-100 dark:hover:bg-gray-700 transition"
          >
            Tutorial Control
          </button>
          <button
            @click="selectedTutorial = 'history'"
            class="w-full text-left p-4 rounded-lg hover:bg-gray-100 dark:hover:bg-gray-700 transition"
          >
            Tutorial History
          </button>
        </div>
      </div>
    </div>
  </transition>

  <!-- Detail Tutorial dengan Video -->
  <transition name="slide">
    <div
      v-if="selectedTutorial"
      class="fixed inset-0 bg-black bg-opacity-60 z-50 backdrop-blur-sm flex items-center justify-center p-4"
      @click.self="selectedTutorial = null"
    >
      <div
        class="w-full max-w-4xl bg-white dark:bg-gray-800 rounded-2xl shadow-2xl overflow-hidden"
      >
        <div
          class="flex justify-between items-center p-6 border-b dark:border-gray-700"
        >
          <h3 class="text-2xl font-bold">
            {{ tutorialTitles[selectedTutorial] }}
          </h3>
          <button
            @click="selectedTutorial = null"
            class="text-gray-500 hover:text-gray-700"
          >
            <svg
              class="w-8 h-8"
              fill="none"
              stroke="currentColor"
              viewBox="0 0 24 24"
            >
              <path
                stroke-linecap="round"
                stroke-linejoin="round"
                stroke-width="2"
                d="M6 18L18 6M6 6l12 12"
              />
            </svg>
          </button>
        </div>
        <div class="p-6 max-h-[80vh] overflow-y-auto">
          <div
            class="relative pt-[56.25%] bg-black rounded-xl overflow-hidden mb-6"
          >
            <video
              :src="`/videos/tutorial-${selectedTutorial}.mp4`"
              controls
              class="absolute inset-0 w-full h-full"
            ></video>
          </div>
          <div class="prose dark:prose-invert max-w-none">
            <p class="text-lg leading-relaxed">
              {{ tutorialDescriptions[selectedTutorial] }}
            </p>
          </div>
        </div>
      </div>
    </div>
  </transition>
</template>

<script setup lang="ts">
import { ref } from "vue";

const isMainVisible = ref(true);
const showTanyaAdmin = ref(false);
const showTutorialMenu = ref(false);
const selectedTutorial = ref<null | string>(null);
const pesan = ref("");

const waNumber = "6285927425788";

const openTanyaAdmin = () => {
  showTanyaAdmin.value = true;
  isMainVisible.value = false;
};

const kirimWA = () => {
  if (!pesan.value.trim()) return;
  const text = encodeURIComponent(
    `Halo Admin, saya mau tanya:\n\n${pesan.value}`
  );
  window.open(`https://wa.me/${waNumber}?text=${text}`, "_blank");
  pesan.value = "";
  showTanyaAdmin.value = false;
  isMainVisible.value = true;
};

const tutorialTitles: Record<string, string> = {
  alat: "Tutorial Penggunaan Alat",
  dashboard: "Tutorial Dashboard Monitoring",
  control: "Tutorial Mode Control (Auto/Manual)",
  history: "Tutorial Melihat Riwayat Pengeringan",
};

const tutorialDescriptions: Record<string, string> = {
  alat: "Video ini menjelaskan cara menghidupkan alat, memasukkan bahan, hingga proses pengeringan selesai. Perhatikan indikator lampu dan pastikan semua sensor terhubung.",
  dashboard:
    "Pelajari cara membaca grafik real-time, memahami status koneksi, dan arti dari setiap sensor (Suhu, Kelembapan, Kadar Air, Berat).",
  control:
    "Pelajari perbedaan mode AUTO dan MANUAL, kapan sistem otomatis mengaktifkan collector, serta cara override manual jika diperlukan.",
  history:
    "Cara melihat riwayat pengeringan, download laporan PDF/Excel, dan analisa performa alat dari waktu ke waktu.",
};
</script>

<style scoped>
.fade-scale-enter-active,
.fade-scale-leave-active {
  transition: all 0.3s ease;
}
.fade-scale-enter-from,
.fade-scale-leave-to {
  opacity: 0;
  transform: scale(0.8) translateY(20px);
}

.slide-enter-active,
.slide-leave-active {
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
}
.slide-enter-from,
.slide-leave-to {
  opacity: 0;
  transform: translateY(100%);
}

.animate-slide-up {
  animation: slideUp 0.4s ease-out;
}
@keyframes slideUp {
  from {
    transform: translateY(100%);
    opacity: 0;
  }
  to {
    transform: translateY(0);
    opacity: 1;
  }
}
</style>
