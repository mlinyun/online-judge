<script setup lang="ts">
import { useRouter } from "vue-router";

defineOptions({ name: "HeroSection" });

const router = useRouter();

// 导航到题目列表页
const navigateToProblemList = () => {
    router.push("/problem/list");
};

// 导航到用户排名页
const navigateToRank = () => {
    router.push("/user/rank");
};
</script>

<template>
    <div class="hero-section">
        <div class="hero-glow"></div>
        <h1 class="hero-title">
            <span class="hero-title-line">Welcome to</span>
            <span class="hero-title-gradient">LYOJ Platform</span>
        </h1>
        <p class="hero-description">下一代在线编程评测系统。沉浸式编码体验，实时评测反馈，探索算法的无限可能。</p>
        <div class="hero-actions">
            <button class="hero-btn hero-btn-primary" @click="navigateToProblemList">
                开始刷题
                <i class="icon-arrow-right"></i>
            </button>
            <button class="hero-btn hero-btn-secondary" @click="navigateToRank">查看排名</button>
        </div>
    </div>
</template>

<style scoped>
.hero-section {
    position: relative;
    padding: var(--oj-padding-xl) var(--oj-padding-md);
    text-align: center;
}

@media (width >= 1024px) {
    .hero-section {
        padding-top: var(--oj-padding-xl);
        padding-bottom: var(--oj-spacing-16);
    }
}

.hero-glow {
    position: absolute;
    top: 50%;
    left: 50%;
    z-index: -1;
    width: 600px;
    height: 600px;
    pointer-events: none;
    background: radial-gradient(circle, rgb(6 182 212 / 15%) 0%, transparent 70%);
    filter: blur(60px);
    transform: translate(-50%, -50%);
}

.hero-title {
    margin-bottom: var(--oj-spacing-4);
    font-size: var(--oj-font-size-4xl);
    font-weight: var(--oj-font-weight-extrabold);
    color: var(--oj-text-color);
    letter-spacing: var(--oj-letter-spacing-tight);
}

@media (width >= 640px) {
    .hero-title {
        font-size: var(--oj-font-size-6xl);
    }
}

.hero-title-line {
    display: block;
}

.hero-title-gradient {
    display: block;
    margin-top: var(--oj-spacing-2);
    background: linear-gradient(to right, var(--oj-color-primary), var(--oj-color-secondary));
    background-clip: text;
    -webkit-text-fill-color: transparent;
}

.hero-description {
    max-width: 48rem;
    margin-top: var(--oj-spacing-4);
    margin-right: auto;
    margin-left: auto;
    font-size: var(--oj-font-size-xl);
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-color-secondary);
}

.hero-actions {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    justify-content: center;
    margin-top: var(--oj-spacing-8);
}

.hero-btn {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    padding: var(--oj-spacing-3) var(--oj-spacing-8);
    font-size: var(--oj-font-size-base);
    font-weight: var(--oj-font-weight-medium);
    cursor: pointer;
    border: 1px solid transparent;
    border-radius: var(--oj-radius-md);
    transition: all 0.3s ease;
}

@media (width >= 768px) {
    .hero-btn {
        padding: var(--oj-spacing-4) var(--oj-spacing-10);
        font-size: var(--oj-font-size-lg);
    }
}

.hero-btn-primary {
    color: var(--oj-text-inverse);
    background-color: var(--oj-color-primary);
    box-shadow: var(--oj-shadow-neon);
}

.hero-btn-primary:hover {
    background-color: var(--oj-color-primary-dark);
    box-shadow:
        0 0 8px var(--oj-color-primary),
        0 0 30px var(--oj-color-primary);
    transform: translateY(-2px);
}

.hero-btn-secondary {
    color: var(--oj-color-primary);
    background-color: transparent;
    border-color: var(--oj-color-primary);
}

.hero-btn-secondary:hover {
    background-color: var(--oj-color-primary-soft);
    transform: translateY(-1px);
}

.icon-arrow-right::before {
    margin-left: var(--oj-spacing-1);
    content: "→";
}
</style>
