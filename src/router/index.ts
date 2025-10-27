// FILE: src/router/index.ts
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

router.beforeEach((to, from, next) => {
  const authStore = useAuthStore();

  if (authStore.loading) {
    setTimeout(() => {
      if (to.meta.requiresAuth && !authStore.user) {
        next("/login");
      } else if (to.meta.requiresGuest && authStore.user) {
        next("/dashboard");
      } else {
        next();
      }
    }, 100);
  } else {
    if (to.meta.requiresAuth && !authStore.user) {
      next("/login");
    } else if (to.meta.requiresGuest && authStore.user) {
      next("/dashboard");
    } else {
      next();
    }
  }
});

export default router;
