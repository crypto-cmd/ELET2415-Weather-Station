// Composables
import { createRouter, createWebHistory } from "vue-router";

const routes = [
  {
    path: "/",
    name: "Home",
    component: () => import("@/views/Home.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/about",
    name: "About",
    component: () => import("@/views/AboutView.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/station",
    name: "Station",
    component: () => import("@/views/StationView.vue"),
    meta: { transition: "fade" },
  },
  {
    path: "/analysis",
    name: "Analysis",
    component: () => import("@/views/AnalysisView.vue"),
    meta: { transition: "fade" },
  },
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes,
});

export default router;
