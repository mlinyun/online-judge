<script setup lang="ts">
defineOptions({ name: "FeatureCards" });

interface Feature {
    icon: string;
    title: string;
    description: string;
    colorClass: string;
}

const features: Feature[] = [
    {
        icon: "database",
        title: "海量题库",
        description: "涵盖基础算法、数据结构到高级专题，满足不同阶段的学习需求。",
        colorClass: "feature-primary",
    },
    {
        icon: "bolt",
        title: "极速评测",
        description: "分布式评测集群，毫秒级响应，支持多种主流编程语言。",
        colorClass: "feature-secondary",
    },
    {
        icon: "users",
        title: "活跃社区",
        description: "分享解题思路，参与技术讨论，与众多开发者共同进步。",
        colorClass: "feature-success",
    },
];
</script>

<template>
    <div class="feature-cards">
        <div v-for="feature in features" :key="feature.title" class="feature-card">
            <div :class="['feature-icon', feature.colorClass]">
                <span :class="`icon-${feature.icon}`"></span>
            </div>
            <h3 class="feature-title">{{ feature.title }}</h3>
            <p class="feature-description">{{ feature.description }}</p>
        </div>
    </div>
</template>

<style scoped>
.feature-cards {
    display: grid;
    grid-template-columns: 1fr;
    gap: var(--oj-spacing-6);
    margin-bottom: var(--oj-spacing-16);
}

@media (width >= 768px) {
    .feature-cards {
        grid-template-columns: repeat(3, 1fr);
    }
}

.feature-card {
    padding: var(--oj-spacing-6);
    background: var(--oj-glass-bg);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
    box-shadow: var(--oj-shadow-md);
    backdrop-filter: blur(var(--oj-glass-blur));
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.feature-card:hover {
    box-shadow: var(--oj-shadow-lg);
    transform: translateY(-4px);
}

.feature-card:nth-child(1):hover {
    border-color: var(--oj-color-primary);
    box-shadow:
        0 0 5px var(--oj-color-primary),
        var(--oj-shadow-lg);
}

.feature-card:nth-child(2):hover {
    border-color: var(--oj-color-secondary);
    box-shadow:
        0 0 5px var(--oj-color-secondary),
        var(--oj-shadow-lg);
}

.feature-card:nth-child(3):hover {
    border-color: var(--oj-color-success);
    box-shadow:
        0 0 5px var(--oj-color-success),
        var(--oj-shadow-lg);
}

.feature-icon {
    display: flex;
    align-items: center;
    justify-content: flex-start;
    margin-bottom: var(--oj-spacing-3);
    font-size: var(--oj-font-size-3xl);
}

.feature-icon.feature-primary {
    color: var(--oj-color-primary);
}

.feature-icon.feature-secondary {
    color: var(--oj-color-secondary);
}

.feature-icon.feature-success {
    color: var(--oj-color-success);
}

.feature-title {
    margin-bottom: var(--oj-spacing-2);
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.feature-description {
    font-size: var(--oj-font-size-sm);
    line-height: var(--oj-line-height-relaxed);
    color: var(--oj-text-color-secondary);
}

/* Icon styles using CSS pseudo-elements */
.icon-database::before {
    content: "🗄️";
}

.icon-bolt::before {
    content: "⚡";
}

.icon-users::before {
    content: "👥";
}
</style>
