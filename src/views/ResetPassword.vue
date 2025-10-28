// FILE: src/views/ResetPassword.vue (NEW) //
============================================
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
              d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z"
            />
          </svg>
        </div>
        <h1 class="text-3xl font-bold text-gray-800 mb-2">Reset Password</h1>
        <p class="text-gray-600">
          {{
            verifiedEmail
              ? `Reset password untuk ${verifiedEmail}`
              : "Masukkan password baru Anda"
          }}
        </p>
      </div>

      <!-- Loading State -->
      <div v-if="verifying" class="text-center py-8">
        <div
          class="inline-block animate-spin rounded-full h-12 w-12 border-b-2 border-primary mb-4"
        ></div>
        <p class="text-gray-600">Memverifikasi kode...</p>
      </div>

      <!-- Success Message -->
      <div
        v-else-if="resetSuccess"
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
            <p class="font-semibold">Password berhasil direset!</p>
            <p class="text-sm mt-1">
              Anda akan diarahkan ke halaman login dalam
              {{ countdown }} detik...
            </p>
          </div>
        </div>
      </div>

      <!-- Reset Form -->
      <form
        v-else-if="verifiedEmail"
        @submit.prevent="handleResetPassword"
        class="space-y-6"
      >
        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2">
            Password Baru
          </label>
          <input
            v-model="newPassword"
            type="password"
            required
            minlength="6"
            class="w-full px-4 py-3 border border-gray-300 rounded-lg focus:ring-2 focus:ring-primary focus:border-transparent transition"
            placeholder="••••••••"
          />
          <p class="text-xs text-gray-500 mt-1">Minimal 6 karakter</p>
        </div>

        <div>
          <label class="block text-sm font-medium text-gray-700 mb-2">
            Konfirmasi Password Baru
          </label>
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
          {{ isLoading ? "Memproses..." : "Reset Password" }}
        </button>
      </form>

      <!-- Error State -->
      <div v-else-if="authStore.error" class="text-center py-8">
        <div
          class="w-16 h-16 bg-red-100 rounded-full flex items-center justify-center mx-auto mb-4"
        >
          <svg
            class="w-8 h-8 text-red-600"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z"
            />
          </svg>
        </div>
        <p class="text-red-600 font-semibold mb-2">{{ authStore.error }}</p>
        <router-link
          to="/forgot-password"
          class="text-primary font-semibold hover:underline"
        >
          Minta link baru
        </router-link>
      </div>

      <div class="mt-6 text-center">
        <router-link
          to="/login"
          class="text-primary font-semibold hover:underline"
        >
          ← Kembali ke Login
        </router-link>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from "vue";
import { useRoute, useRouter } from "vue-router";
import { useAuthStore } from "@/stores/auth";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();

const verifying = ref(true);
const verifiedEmail = ref<string | null>(null);
const newPassword = ref("");
const confirmPassword = ref("");
const isLoading = ref(false);
const resetSuccess = ref(false);
const countdown = ref(3);

const oobCode = computed(() => route.query.oobCode as string);

const error = computed(() => {
  if (
    newPassword.value &&
    confirmPassword.value &&
    newPassword.value !== confirmPassword.value
  ) {
    return "Password tidak sama";
  }
  return authStore.error;
});

onMounted(async () => {
  if (!oobCode.value) {
    authStore.error = "Kode reset tidak valid";
    verifying.value = false;
    return;
  }

  // Verify the reset code
  const email = await authStore.verifyResetCode(oobCode.value);
  verifiedEmail.value = email;
  verifying.value = false;
});

const handleResetPassword = async () => {
  if (newPassword.value !== confirmPassword.value) {
    return;
  }

  if (!oobCode.value) {
    return;
  }

  isLoading.value = true;
  try {
    const success = await authStore.resetPassword(
      oobCode.value,
      newPassword.value
    );

    if (success) {
      resetSuccess.value = true;

      // Countdown and redirect
      const interval = setInterval(() => {
        countdown.value--;
        if (countdown.value === 0) {
          clearInterval(interval);
          router.push("/login");
        }
      }, 1000);
    }
  } catch (error) {
    console.error("Reset password error:", error);
  } finally {
    isLoading.value = false;
  }
};
</script>
