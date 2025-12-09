<script setup lang="ts">
/**
 * 管理后台 - 头部组件
 */
import { useRoute } from "vue-router";
import { ThemeToggle, UserDropdown } from "@/components/main";

defineOptions({ name: "AdminHeader" });

const route = useRoute();
</script>

<template>
    <header class="admin-header">
        <!-- 左侧: 标题 -->
        <div class="header-left">
            <h1 class="page-title">{{ route.meta?.title || "管理后台" }}</h1>
        </div>

        <!-- 右侧: 工具按钮和用户信息 -->
        <div class="header-right">
            <!-- 通知按钮 -->
            <el-button class="header-btn">
                <el-badge :value="2" :max="99">
                    <el-icon :size="18">
                        <i-ep-bell />
                    </el-icon>
                </el-badge>
            </el-button>

            <!-- 主题切换 -->
            <ThemeToggle />

            <!-- 用户下拉菜单 -->
            <UserDropdown />
        </div>
    </header>
</template>

<style scoped>
/* 顶部工具栏 */
.admin-header {
    position: sticky;
    top: 0;
    z-index: var(--oj-z-header);
    display: flex;
    align-items: center;
    justify-content: space-between;
    height: var(--oj-navbar-height);
    padding: 0 var(--oj-spacing-8);
    background: var(--oj-glass-bg);
    border-bottom: 1px solid var(--oj-glass-border);
    backdrop-filter: blur(12px);
}

.header-left {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
}

.page-title {
    font-size: var(--oj-font-size-xl);
    font-weight: 700;
    color: var(--oj-text-color);
}

.header-right {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
}

.header-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 36px;
    height: 36px;
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: transparent;
    border: none;
    border-radius: var(--oj-radius-md);
    transition: all 0.2s;
}

.header-btn:hover {
    color: var(--oj-text-color);
    background: var(--oj-bg-hover);
}

.admin-user {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
}
</style>
