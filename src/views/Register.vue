// FILE: src/views/Register.vue // ============================================
<template>
  <div
    class="min-h-screen flex items-center justify-center bg-gradient-to-br from-red-50 to-gray-100"
  >
    <div class="bg-white p-8 rounded-2xl shadow-2xl w-full max-w-md">
      <div class="text-center mb-8">
        <h1 class="text-3xl font-bold text-gray-800 mb-2">PENGERING CABAI</h1>
        <p class="text-gray-600">Buat akun baru</p>
      </div>

      <form @submit.prevent="handleRegister" class="space-y-6">
        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2"
            >Email</label
          >
          <input
            v-model="email"
            type="email"
            required
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="nama@email.com"
          />
        </div>

        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2"
            >Password</label
          >
          <input
            v-model="password"
            type="password"
            required
            minlength="6"
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="••••••••"
          />
          <p class="text-xs text-gray-500 mt-1">Minimal 6 karakter</p>
        </div>

        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2"
            >Konfirmasi Password</label
          >
          <input
            v-model="confirmPassword"
            type="password"
            required
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="••••••••"
          />
        </div>

        <div
          v-if="error"
          class="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded-lg"
        >
          {{ error }}
        </div>

        <button
          type="submit"
          :disabled="isLoading"
          class="w-full bg-primary text-white py-3 rounded-lg font-semibold hover:bg-red-700 transition disabled:opacity-50 disabled:cursor-not-allowed"
        >
          {{ isLoading ? "Memproses..." : "Daftar" }}
        </button>
      </form>

      <div class="mt-6 text-center">
        <p class="text-gray-600">
          Sudah punya akun?
          <router-link
            to="/login"
            class="text-primary font-semibold hover:underline"
          >
            Masuk di sini
          </router-link>
        </p>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from "vue";
import { useAuthStore } from "@/stores/auth";

const authStore = useAuthStore();
const email = ref("");
const password = ref("");
const confirmPassword = ref("");
const isLoading = ref(false);

const error = computed(() => {
  if (
    password.value &&
    confirmPassword.value &&
    password.value !== confirmPassword.value
  ) {
    return "Password tidak sama";
  }
  return authStore.error;
});

const handleRegister = async () => {
  if (password.value !== confirmPassword.value) {
    return;
  }

  isLoading.value = true;
  try {
    await authStore.register(email.value, password.value);
  } catch (error) {
    console.error("Register error:", error);
  } finally {
    isLoading.value = false;
  }
};
</script>
