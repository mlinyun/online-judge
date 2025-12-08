// 全局样式 (设计 Token + 主题系统 + Element Plus 覆盖)
import "./styles/index.css";
import App from "./App.vue";
import { createApp } from "vue";
import router from "./router";
import { initPiniaStore } from "@/stores"; // 初始化 Pinia Store

const app = createApp(App);
initPiniaStore(app); // 使用 Pinia Store

app.use(router);

app.mount("#app");
