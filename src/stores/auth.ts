// FILE: src/stores/auth.ts (UPDATED with Password Reset)
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";
import {
  signInWithEmailAndPassword,
  createUserWithEmailAndPassword,
  signOut,
  onAuthStateChanged,
  sendPasswordResetEmail,
  confirmPasswordReset,
  verifyPasswordResetCode,
  User,
} from "firebase/auth";
import { auth } from "@/firebase/config";
import { useRouter } from "vue-router";

export const useAuthStore = defineStore("auth", () => {
  const user = ref<User | null>(null);
  const loading = ref(true);
  const error = ref<string | null>(null);
  const resetEmailSent = ref(false);

  const router = useRouter();

  onAuthStateChanged(auth, (firebaseUser) => {
    user.value = firebaseUser;
    loading.value = false;
  });

  const login = async (email: string, password: string) => {
    try {
      error.value = null;
      await signInWithEmailAndPassword(auth, email, password);
      router.push("/dashboard");
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
      throw e;
    }
  };

  const register = async (email: string, password: string) => {
    try {
      error.value = null;
      await createUserWithEmailAndPassword(auth, email, password);
      router.push("/dashboard");
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
      throw e;
    }
  };

  const logout = async () => {
    try {
      await signOut(auth);
      router.push("/login");
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
    }
  };

  // Kirim email reset password
  const sendPasswordReset = async (email: string) => {
    try {
      error.value = null;
      resetEmailSent.value = false;

      await sendPasswordResetEmail(auth, email, {
        url: window.location.origin + "/login",
        handleCodeInApp: false,
      });

      resetEmailSent.value = true;
      return true;
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
      return false;
    }
  };

  // Verifikasi kode OTP dari email
  const verifyResetCode = async (code: string) => {
    try {
      error.value = null;
      const email = await verifyPasswordResetCode(auth, code);
      return email;
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
      return null;
    }
  };

  // Reset password dengan kode OTP
  const resetPassword = async (code: string, newPassword: string) => {
    try {
      error.value = null;
      await confirmPasswordReset(auth, code, newPassword);
      return true;
    } catch (e: any) {
      error.value = getErrorMessage(e.code);
      return false;
    }
  };

  const getErrorMessage = (code: string): string => {
    switch (code) {
      case "auth/invalid-email":
        return "Email tidak valid";
      case "auth/user-disabled":
        return "Akun dinonaktifkan";
      case "auth/user-not-found":
        return "Email tidak terdaftar";
      case "auth/wrong-password":
        return "Password salah";
      case "auth/email-already-in-use":
        return "Email sudah terdaftar";
      case "auth/weak-password":
        return "Password terlalu lemah (minimal 6 karakter)";
      case "auth/invalid-action-code":
        return "Kode tidak valid atau sudah kadaluarsa";
      case "auth/expired-action-code":
        return "Kode sudah kadaluarsa. Silakan minta kode baru";
      case "auth/too-many-requests":
        return "Terlalu banyak percobaan. Coba lagi nanti";
      default:
        return "Terjadi kesalahan";
    }
  };

  return {
    user,
    loading,
    error,
    resetEmailSent,
    login,
    register,
    logout,
    sendPasswordReset,
    verifyResetCode,
    resetPassword,
  };
});
