// FILE: src/router/index.ts (FIXED - Proper route handling)
// ============================================
import { createRouter, createWebHistory } from "vue-router";
import { useAuthStore } from "@/stores/auth";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      redirect: "/dashboard",
    },
    {
      path: "/login",
      name: "Login",
      component: () => import("@/views/Login.vue"),
      meta: { requiresGuest: true },
    },
    {
      path: "/register",
      name: "Register",
      component: () => import("@/views/Register.vue"),
      meta: { requiresGuest: true },
    },
    {
      path: "/forgot-password",
      name: "ForgotPassword",
      component: () => import("@/views/ForgotPassword.vue"),
      // Tidak pakai requiresGuest agar bisa diakses kapan saja
    },
    {
      path: "/reset-password",
      name: "ResetPassword",
      component: () => import("@/views/ResetPassword.vue"),
      // Tidak pakai requiresGuest agar bisa diakses dari link email
    },
    {
      path: "/dashboard",
      name: "Dashboard",
      component: () => import("@/views/Dashboard.vue"),
      meta: { requiresAuth: true },
    },
    {
      path: "/control",
      name: "Control",
      component: () => import("@/views/Control.vue"),
      meta: { requiresAuth: true },
    },
    {
      path: "/history",
      name: "History",
      component: () => import("@/views/History.vue"),
      meta: { requiresAuth: true },
    },
  ],
});

router.beforeEach(async (to, from, next) => {
  const authStore = useAuthStore();

  // Tunggu hingga auth state loading selesai
  if (authStore.loading) {
    // Gunakan Promise untuk menunggu loading selesai
    await new Promise<void>((resolve) => {
      const checkLoading = setInterval(() => {
        if (!authStore.loading) {
          clearInterval(checkLoading);
          resolve();
        }
      }, 50);
    });
  }

  const isAuthenticated = !!authStore.user;

  // Halaman yang memerlukan autentikasi
  if (to.meta.requiresAuth && !isAuthenticated) {
    next("/login");
    return;
  }

  // Halaman guest (login, register) - redirect jika sudah login
  if (to.meta.requiresGuest && isAuthenticated) {
    next("/dashboard");
    return;
  }

  next();
});

export default router;
