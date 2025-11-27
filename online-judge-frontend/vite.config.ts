import { fileURLToPath, URL } from "node:url";

import { defineConfig, ProxyOptions } from "vite";
import vue from "@vitejs/plugin-vue";
import vueDevTools from "vite-plugin-vue-devtools";

// https://vite.dev/config/
export default defineConfig({
    plugins: [vue(), vueDevTools()],
    resolve: {
        alias: {
            "@": fileURLToPath(new URL("./src", import.meta.url)),
        },
    },
    server: {
        port: 8080,
        open: true,
        proxy: {
            "/api": {
                target: "http://127.0.0.1:8081/api",
                changeOrigin: true,
                rewrite: (path) => path.replace(/^\/api/, ""),
            } as ProxyOptions,
        },
    },
});
