<template>
  <div
    class="min-h-screen flex items-center justify-center bg-gradient-to-br from-red-50 to-gray-100"
  >
    <div class="bg-white p-8 rounded-2xl shadow-2xl w-full max-w-md">
      <div class="text-center mb-8">
        <div
          class="w-16 h-16 bg-red-100 rounded-full flex items-center justify-center mx-auto mb-4"
        >
          <svg
            class="w-8 h-8 text-primary"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M15 7a2 2 0 012 2m4 0a6 6 0 01-7.743 5.743L11 17H9v2H7v2H4a1 1 0 01-1-1v-2.586a1 1 0 01.293-.707l5.964-5.964A6 6 0 1121 9z"
            />
          </svg>
        </div>
        <h1 class="text-3xl font-bold text-gray-800 mb-2">Lupa Password?</h1>
        <p class="text-gray-600">
          Masukkan email Anda untuk menerima link reset password
        </p>
      </div>

      <!-- Success Message -->
      <div
        v-if="authStore.resetEmailSent"
        class="mb-6 bg-green-50 border border-green-200 text-green-700 px-4 py-3 rounded-lg"
      >
        <div class="flex items-start">
          <svg
            class="w-5 h-5 mr-2 mt-0.5"
            fill="currentColor"
            viewBox="0 0 20 20"
          >
            <path
              fill-rule="evenodd"
              d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z"
              clip-rule="evenodd"
            />
          </svg>
          <div>
            <p class="font-semibold">Email berhasil dikirim!</p>
            <p class="text-sm mt-1">
              Silakan cek email Anda dan klik link yang dikirimkan untuk reset
              password. Link akan kadaluarsa dalam 1 jam.
            </p>
          </div>
        </div>
      </div>

      <form
        v-if="!authStore.resetEmailSent"
        @submit.prevent="handleForgotPassword"
        class="space-y-6"
      >
        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2">
            Email
          </label>
          <input
            v-model="email"
            type="email"
            required
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="nama@email.com"
          />
        </div>

        <div
          v-if="authStore.error"
          class="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded-lg"
        >
          {{ authStore.error }}
        </div>

        <button
          type="submit"
          :disabled="isLoading"
          class="w-full bg-primary text-white py-3 rounded-lg font-semibold hover:bg-red-700 transition disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {{ isLoading ? "Mengirim..." : "Kirim Link Reset" }}
        </button>
      </form>

      <div class="mt-6 text-center space-y-2">
        <router-link
          to="/login"
          class="block text-primary font-semibold hover:underline"
        >
          ← Kembali ke Login
        </router-link>
        <p class="text-gray-600 text-sm">
          Belum punya akun?
          <router-link
            to="/register"
            class="text-primary font-semibold hover:underline"
          >
            Daftar di sini
          </router-link>
        </p>
      </div>

      <!-- Info Box -->
      <div class="mt-6 p-4 bg-blue-50 rounded-lg">
        <p class="text-xs text-gray-700">
          <strong>Catatan:</strong> Jika Anda tidak menerima email dalam 5
          menit, periksa folder spam atau coba kirim ulang.
        </p>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
import { useAuthStore } from "@/stores/auth";

const authStore = useAuthStore();
const email = ref("");
const isLoading = ref(false);

const handleForgotPassword = async () => {
  isLoading.value = true;
  try {
    await authStore.sendPasswordReset(email.value);
  } catch (error) {
    console.error("Forgot password error:", error);
  } finally {
    isLoading.value = false;
  }
};
</script>
