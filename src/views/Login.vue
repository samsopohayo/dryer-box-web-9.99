// FILE: src/views/Login.vue (UPDATED with Reset Button) //
============================================
<template>
  <div
    class="min-h-screen flex items-center justify-center bg-gradient-to-br from-red-50 to-gray-100"
  >
    <div class="bg-white p-8 rounded-2xl shadow-2xl w-full max-w-md">
      <div class="text-center mb-8">
        <h1 class="text-3xl font-bold text-gray-800 mb-2">PENGERING CABAI</h1>
        <p class="text-gray-600">Masuk ke akun Anda</p>
      </div>

      <form @submit.prevent="handleLogin" class="space-y-6">
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
          <div class="flex items-center justify-between mb-2">
            <label class="block text-sm font-medium text-gray-700"
              >Password</label
            >
            <router-link
              to="/forgot-password"
              class="text-sm text-primary hover:underline font-medium"
            >
              Lupa password?
            </router-link>
          </div>
          <input
            v-model="password"
            type="password"
            required
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="••••••••"
          />
        </div>

        <div
          v-if="authStore.error"
          class="bg-red-50 border border-red-200 text-red-700 px-4 py-3 rounded-lg"
        >
          {{ authStore.error }}
        </div>

        <div class="flex gap-3">
          <button
            type="button"
            @click="handleReset"
            class="flex-1 bg-primary text-white py-3 rounded-lg font-semibold hover:bg-red-700 transition"
          >
            Reset
          </button>
          <button
            type="submit"
            :disabled="isLoading"
            class="flex-1 bg-gray-500 text-white py-3 rounded-lg font-semibold hover:bg-gray-600 transition disabled:opacity-50 disabled:cursor-not-allowed"
          >
            {{ isLoading ? "Memproses..." : "Masuk" }}
          </button>
        </div>
      </form>

      <div class="mt-6 text-center">
        <p class="text-gray-600">
          Belum punya akun?
          <router-link
            to="/register"
            class="text-primary font-semibold hover:underline"
          >
            Daftar di sini
          </router-link>
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
const password = ref("");
const isLoading = ref(false);

const handleLogin = async () => {
  isLoading.value = true;
  try {
    await authStore.login(email.value, password.value);
  } catch (error) {
    console.error("Login error:", error);
  } finally {
    isLoading.value = false;
  }
};

const handleReset = () => {
  email.value = "";
  password.value = "";
  authStore.error = null;
};
</script>
