<script setup lang="ts">
/**
 * 管理后台布局组件
 * 包含左侧导航菜单、顶部工具栏、主内容区
 */
import { ref } from "vue";
import { RouterView } from "vue-router";
import { AdminAside, AdminHeader } from "@/components/admin";
import { STORAGE_KEYS } from "@/constants/config.ts";

defineOptions({ name: "AdminLayout" });

// 状态提升：侧边栏折叠状态
const isCollapsed = ref(false);

// 初始化折叠状态（从本地存储获取）
onMounted(() => {
    // 从本地存储获取折叠状态
    const storedCollapsed = localStorage.getItem(STORAGE_KEYS.SIDEBAR_COLLAPSED);
    if (storedCollapsed !== null) {
        isCollapsed.value = JSON.parse(storedCollapsed);
    }
});

// 监听折叠状态变化（用于折叠状态持久化）
watch(isCollapsed, (newVal) => {
    // 保存折叠状态到本地存储
    localStorage.setItem(STORAGE_KEYS.SIDEBAR_COLLAPSED, JSON.stringify(newVal));
});

// 根据折叠状态计算侧边栏宽度
const sidebarWidth = computed(() => (isCollapsed.value ? "64px" : "224px"));
</script>

<template>
    <div class="admin-layout">
        <!-- 侧边栏 -->
        <AdminAside v-model:collapsed="isCollapsed" />

        <!-- 主内容区 -->
        <div class="admin-main" :style="{ marginLeft: sidebarWidth }">
            <!-- 顶部工具栏 -->
            <AdminHeader />

            <!-- 内容区域 -->
            <main class="admin-content">
                <RouterView />
            </main>
        </div>
    </div>
</template>

<style scoped>
.admin-layout {
    display: flex;
    min-height: 100vh;
}

/* 主内容区 */
.admin-main {
    display: flex;
    flex: 1;
    flex-direction: column;
    min-width: 0;
    transition: margin-left 0.3s ease;
}

/* 内容区域 */
.admin-content {
    flex: 1;
    padding: var(--oj-spacing-8);
    overflow-y: auto;
}
</style>
