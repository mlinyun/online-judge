<script setup lang="ts">
/**
 * 主布局组件
 * 用于普通用户页面，包含顶部导航栏、主内容区、底部页脚
 */
import { RouterView } from "vue-router";
import { MainHeader, MainFooter } from "@/components/main";

defineOptions({ name: "MainLayout" });
</script>

<template>
    <div class="main-layout">
        <!-- 顶部导航栏 -->
        <MainHeader />

        <!-- 主内容区域 -->
        <main class="main-content">
            <div class="main-container">
                <RouterView />
            </div>
        </main>

        <!-- 底部页脚 -->
        <MainFooter />
    </div>
</template>

<style scoped>
.main-layout {
    display: flex;
    flex-direction: column;
    min-height: 100vh;
}

.main-content {
    flex: 1;
    padding-top: var(--oj-navbar-height);
    padding-bottom: var(--oj-spacing-10);
}

.main-container {
    width: 100%;
    max-width: var(--oj-container-xl);
    padding-right: var(--oj-spacing-4);
    padding-left: var(--oj-spacing-4);
    margin-right: auto;
    margin-left: auto;
}

@media (width >=640px) {
    .main-container {
        padding-right: var(--oj-spacing-6);
        padding-left: var(--oj-spacing-6);
    }
}

@media (width >=1024px) {
    .main-container {
        padding-right: var(--oj-spacing-8);
        padding-left: var(--oj-spacing-8);
    }
}
</style>
