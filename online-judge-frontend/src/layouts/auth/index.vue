<script setup lang="ts">
/**
 * 认证布局组件
 * 用于登录、注册等认证页面，居中卡片式布局
 */
import { RouterView } from "vue-router";
import { useRouter } from "vue-router";
import LogoSvg from "@/assets/logo.svg?component";

defineOptions({ name: "AuthLayout" });

const router = useRouter();

const goHome = () => {
    router.push("/");
};
</script>

<template>
    <div class="auth-layout">
        <!-- 背景装饰 -->
        <div class="auth-bg">
            <div class="bg-gradient bg-gradient-1"></div>
            <div class="bg-gradient bg-gradient-2"></div>
        </div>

        <!-- 顶部 Logo -->
        <header class="auth-header">
            <div class="brand" @click="goHome">
                <LogoSvg class="brand-logo" />
                <span class="brand-text"> LY<span class="brand-accent">OJ</span> </span>
            </div>
        </header>

        <!-- 内容区域 -->
        <main class="auth-main">
            <div class="auth-card">
                <RouterView />
            </div>
        </main>

        <!-- 底部信息 -->
        <footer class="auth-footer">
            <p class="copyright">© {{ new Date().getFullYear() }} LYOJ. All rights reserved.</p>
        </footer>
    </div>
</template>

<style scoped>
.auth-layout {
    position: relative;
    display: flex;
    flex-direction: column;
    min-height: 100vh;
    overflow: hidden;
}

/* 背景装饰 */
.auth-bg {
    position: fixed;
    inset: 0;
    z-index: -1;
    pointer-events: none;
}

.bg-gradient {
    position: absolute;
    border-radius: 50%;
    filter: blur(100px);
}

.bg-gradient-1 {
    top: -20%;
    left: -10%;
    width: 600px;
    height: 600px;
    background: rgb(var(--oj-color-primary-rgb) / 15%);
}

.bg-gradient-2 {
    right: -10%;
    bottom: -20%;
    width: 600px;
    height: 600px;
    background: rgb(var(--oj-color-secondary-rgb) / 15%);
}

/* 顶部 Logo */
.auth-header {
    display: flex;
    justify-content: center;
    padding: var(--oj-spacing-8);
}

.brand {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    cursor: pointer;
    transition: transform 0.2s;
}

.brand:hover {
    transform: scale(1.05);
}

.brand-logo {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    animation: oj-pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite;
}

.brand-text {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-2xl);
    font-weight: 700;
    color: var(--oj-text-color);
    letter-spacing: 0.05em;
}

.brand-accent {
    color: var(--oj-color-primary);
}

/* 内容区域 */
.auth-main {
    display: flex;
    flex: 1;
    align-items: center;
    justify-content: center;
    padding: var(--oj-spacing-4);
}

.auth-card {
    width: 100%;
    padding: var(--oj-spacing-8);
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(12px);
}

@media (width >=640px) {
    .auth-card {
        padding: var(--oj-spacing-10);
    }
}

/* 底部信息 */
.auth-footer {
    padding: var(--oj-spacing-6);
    text-align: center;
}

.copyright {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-muted);
}
</style>
