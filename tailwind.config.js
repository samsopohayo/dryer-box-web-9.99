// FILE: tailwind.config.js (UPDATED with dark mode)
// ============================================
/** @type {import('tailwindcss').Config} */
export default {
  content: ["./index.html", "./src/**/*.{vue,js,ts,jsx,tsx}"],
  darkMode: "class",
  theme: {
    extend: {
      colors: {
        primary: "#dc2626",
        secondary: "#374151",
      },
    },
  },
  plugins: [],
};
