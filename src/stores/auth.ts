// FILE: src/stores/auth.ts
// ============================================
import { defineStore } from "pinia";
import { ref } from "vue";
import {
  signInWithEmailAndPassword,
  createUserWithEmailAndPassword,
  signOut,
  onAuthStateChanged,
  User,
} from "firebase/auth";
import { auth } from "@/firebase/config";
import { useRouter } from "vue-router";

export const useAuthStore = defineStore("auth", () => {
  const user = ref<User | null>(null);
  const loading = ref(true);
  const error = ref<string | null>(null);

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
        return "Password terlalu lemah";
      default:
        return "Terjadi kesalahan";
    }
  };

  return {
    user,
    loading,
    error,
    login,
    register,
    logout,
  };
});
