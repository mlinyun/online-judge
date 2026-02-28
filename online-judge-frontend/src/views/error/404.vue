<script setup lang="ts">
/**
 * 404 页面
 *
 * @description 赛博朋克风格的 404 页面
 */
defineOptions({ name: "NotFound" });

const router = useRouter();

const goHome = () => router.push("/");
const goBack = () => router.back();
</script>

<template>
    <div class="not-found-page">
        <div class="not-found-content">
            <!-- 故障风格 404 -->
            <div class="glitch-wrapper">
                <div class="glitch" data-text="404">404</div>
            </div>

            <h2 class="not-found-title">页面未找到</h2>
            <p class="not-found-desc">你访问的页面不存在或已被移除</p>

            <div class="not-found-actions">
                <el-button type="primary" class="action-btn" @click="goHome">
                    <el-icon><i-ep-house /></el-icon>
                    <span>返回首页</span>
                </el-button>
                <el-button class="action-btn ghost-btn" @click="goBack">
                    <el-icon><i-ep-back /></el-icon>
                    <span>返回上一页</span>
                </el-button>
            </div>
        </div>

        <!-- 装饰元素 -->
        <div class="circuit-lines" />
    </div>
</template>

<style scoped>
.not-found-page {
    position: relative;
    display: flex;
    align-items: center;
    justify-content: center;
    min-height: 100vh;
    overflow: hidden;
    background: var(--oj-bg-base);
}

.not-found-content {
    position: relative;
    z-index: 1;
    text-align: center;
}

/* ── Glitch 404 ── */
.glitch-wrapper {
    margin-bottom: var(--oj-spacing-4);
}

.glitch {
    position: relative;
    font-size: clamp(80px, 20vw, 160px);
    font-weight: 900;
    line-height: 1;
    color: var(--oj-text-color);
    text-shadow:
        0 0 40px rgb(var(--oj-color-primary-rgb) / 40%),
        0 0 80px rgb(var(--oj-color-primary-rgb) / 20%);
}

.glitch::before,
.glitch::after {
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    content: attr(data-text);
}

.glitch::before {
    color: rgb(6 182 212 / 80%);
    clip-path: polygon(0 0, 100% 0, 100% 33%, 0 33%);
    animation: glitch-top 2.5s infinite linear alternate-reverse;
}

.glitch::after {
    color: rgb(248 113 113 / 80%);
    clip-path: polygon(0 67%, 100% 67%, 100% 100%, 0 100%);
    animation: glitch-bottom 3s infinite linear alternate-reverse;
}

@keyframes glitch-top {
    0%,
    90%,
    100% {
        transform: translate(0);
    }

    92% {
        transform: translate(-5px, 2px);
    }

    94% {
        transform: translate(5px, -2px);
    }

    96% {
        transform: translate(-3px, 1px);
    }

    98% {
        transform: translate(3px, -1px);
    }
}

@keyframes glitch-bottom {
    0%,
    88%,
    100% {
        transform: translate(0);
    }

    90% {
        transform: translate(4px, -3px);
    }

    93% {
        transform: translate(-4px, 3px);
    }

    95% {
        transform: translate(2px, -1px);
    }

    97% {
        transform: translate(-2px, 1px);
    }
}

.not-found-title {
    margin: 0 0 var(--oj-spacing-2);
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.not-found-desc {
    margin: 0 0 var(--oj-spacing-8);
    font-size: var(--oj-font-size-base);
    color: var(--oj-text-color-secondary);
}

/* ── 按钮 ── */
.not-found-actions {
    display: flex;
    gap: var(--oj-spacing-3);
    justify-content: center;
}

.action-btn {
    min-width: 140px;
    font-weight: 600;
}

.action-btn:first-child:hover {
    box-shadow: 0 0 24px rgb(var(--oj-color-primary-rgb) / 40%);
}

.ghost-btn {
    color: var(--oj-text-color-secondary);
    background: transparent;
    border: 1px solid var(--oj-card-border);
}

.ghost-btn:hover {
    color: var(--oj-text-color);
    border-color: var(--oj-color-primary);
}

/* ── 装饰电路线 ── */
.circuit-lines {
    position: absolute;
    inset: 0;
    background-image:
        linear-gradient(rgb(var(--oj-color-primary-rgb) / 3%) 1px, transparent 1px),
        linear-gradient(90deg, rgb(var(--oj-color-primary-rgb) / 3%) 1px, transparent 1px);
    background-size: 60px 60px;
    mask-image: radial-gradient(ellipse at center, rgb(0 0 0 / 30%) 0%, transparent 70%);
    pointer-events: none;
}
</style>
