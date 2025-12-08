<script setup lang="ts">
import { onMounted } from "vue";
import { RouterView } from "vue-router";
import { ElConfigProvider } from "element-plus";
import zhCn from "element-plus/es/locale/lang/zh-cn";
import { useTheme } from "@/composables/useTheme";

// 初始化主题
const { initTheme } = useTheme();

onMounted(() => {
    initTheme();
});
</script>

<template>
    <!-- Element Plus 提供了一个 Vue 组件 ConfigProvider 用于全局配置国际化的设置 -->
    <el-config-provider :locale="zhCn">
        <RouterView />
    </el-config-provider>
</template>

<style scoped></style>
