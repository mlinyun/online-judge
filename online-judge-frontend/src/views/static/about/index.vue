<script setup lang="ts">
defineOptions({ name: "AboutPage" });

const overviewItems = [
    {
        title: "评测引擎",
        desc: "多语言隔离运行，毫秒级反馈。",
    },
    {
        title: "学习闭环",
        desc: "题库 + 题解 + 讨论一体联动。",
    },
    {
        title: "竞赛支持",
        desc: "赛制配置、排行榜与赛后复盘。",
    },
];

const pillars = [
    {
        title: "评测可信",
        desc: "隔离运行环境与多维测试点，稳定输出可复盘的评测结果。",
    },
    {
        title: "学习闭环",
        desc: "题库、讨论、题解与记录串联，帮助学习者持续迭代。",
    },
    {
        title: "教学友好",
        desc: "支持课程与竞赛组织，掌控进度与统计表现。",
    },
];

const features = [
    "低延迟评测，结果反馈更快",
    "多角色协作：出题、教学、竞赛一体化",
    "成长数据可视化，训练路径一目了然",
    "安全与权限体系，保障内容与数据",
];
</script>

<template>
    <section class="promo-page about-page" aria-label="关于我们">
        <header class="hero">
            <div class="hero-content">
                <div class="eyebrow">About LYOJ</div>
                <h1 class="hero-title">为学习与竞赛而生的在线评测平台</h1>
                <p class="hero-subtitle">
                    我们打造稳定、公平、可扩展的评测基础设施，把高质量编程训练带到每一所学校和每一位学习者身边。
                </p>
                <div class="hero-actions">
                    <router-link to="/problem/list" class="btn btn-primary">探索题库</router-link>
                    <router-link to="/auth/register" class="btn btn-ghost">加入我们</router-link>
                </div>
            </div>
            <div class="hero-card">
                <div class="hero-card-title">平台概览</div>
                <div class="hero-card-panel">
                    <div class="overview-stack">
                        <div v-for="item in overviewItems" :key="item.title" class="overview-item">
                            <div class="overview-title">{{ item.title }}</div>
                            <div class="overview-desc">{{ item.desc }}</div>
                        </div>
                    </div>
                    <div class="overview-divider"></div>
                    <div class="overview-note">统一身份体系 + 可视化成长路径 + 稳定的判题基础设施</div>
                </div>
            </div>
        </header>

        <section class="section">
            <div class="section-title">核心能力</div>
            <div class="grid">
                <div v-for="pillar in pillars" :key="pillar.title" class="card">
                    <div class="card-title">{{ pillar.title }}</div>
                    <p class="card-text">{{ pillar.desc }}</p>
                </div>
            </div>
        </section>

        <section class="section">
            <div class="section-title">为什么选择 LYOJ</div>
            <div class="split">
                <div class="card">
                    <div class="card-title">全链路学习体验</div>
                    <p class="card-text">从入门题到竞赛题，结合题解、讨论与评测记录，形成清晰的学习与复盘流程。</p>
                </div>
                <div class="card">
                    <ul class="list">
                        <li v-for="item in features" :key="item">{{ item }}</li>
                    </ul>
                </div>
            </div>
        </section>

        <section class="cta">
            <div>
                <div class="cta-title">准备好体验更高效的训练平台了吗？</div>
                <div class="cta-subtitle">快速创建账号，开启你的编程训练计划。</div>
            </div>
            <router-link to="/auth/register" class="btn btn-primary">立即开始</router-link>
        </section>
    </section>
</template>

<style scoped>
@import url("https://fonts.googleapis.com/css2?family=Nunito+Sans:wght@300;400;500;600;700&family=Rubik:wght@300;400;500;600;700&display=swap");

.promo-page {
    --promo-bg:
        var(--oj-gradient-hero),
        linear-gradient(180deg, var(--oj-bg-base) 0%, var(--oj-bg-dark) 55%, var(--oj-bg-base) 100%);
    --promo-card: color-mix(in srgb, var(--oj-bg-light) 72%, transparent);
    --promo-border: var(--oj-border-base);
    --promo-text: var(--oj-text-primary);
    --promo-text-muted: var(--oj-text-secondary);
    --promo-accent: var(--oj-color-primary);
    --promo-accent-soft: var(--oj-color-primary-soft);
    color: var(--promo-text);
    font-family: "Nunito Sans", "Rubik", system-ui, sans-serif;
    background: var(--promo-bg);
    border-radius: var(--oj-radius-2xl);
    padding: clamp(24px, 4vw, 48px);
    display: flex;
    flex-direction: column;
    gap: clamp(24px, 4vw, 48px);
}

.hero {
    display: grid;
    gap: clamp(24px, 4vw, 40px);
    grid-template-columns: minmax(0, 1.15fr) minmax(0, 0.85fr);
    padding: clamp(24px, 4vw, 40px);
    border-radius: var(--oj-radius-2xl);
    background: color-mix(in srgb, var(--oj-bg-base) 70%, transparent);
    border: 1px solid var(--promo-border);
    position: relative;
    overflow: hidden;
    align-items: stretch;
}

.hero::after {
    content: "";
    position: absolute;
    inset: 0;
    background: linear-gradient(120deg, var(--oj-color-primary-soft), transparent 45%);
    opacity: 0.7;
    pointer-events: none;
}

.hero-content,
.hero-card {
    position: relative;
    z-index: 1;
}

.hero-content {
    display: flex;
    flex-direction: column;
    justify-content: center;
    gap: 8px;
    max-width: 560px;
}

.eyebrow {
    font-size: 12px;
    letter-spacing: 0.2em;
    text-transform: uppercase;
    color: var(--promo-text-muted);
}

.hero-title {
    font-family: "Rubik", "Nunito Sans", sans-serif;
    font-size: clamp(28px, 3.6vw, 44px);
    margin: 12px 0 8px;
    text-shadow: 0 0 14px rgba(var(--oj-color-primary-rgb) / 30%);
}

.hero-subtitle {
    font-size: 16px;
    line-height: 1.7;
    color: var(--promo-text-muted);
    max-width: 520px;
}

.hero-actions {
    display: flex;
    gap: 12px;
    margin-top: 20px;
    flex-wrap: wrap;
}

.btn {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    padding: 12px 20px;
    border-radius: 999px;
    font-weight: 600;
    text-decoration: none;
    transition:
        transform 0.2s ease,
        box-shadow 0.2s ease,
        background 0.2s ease;
    cursor: pointer;
    min-height: 44px;
}

.btn:focus-visible {
    outline: 2px solid var(--promo-accent);
    outline-offset: 2px;
}

.btn-primary {
    background: var(--promo-accent);
    color: var(--oj-bg-base);
    box-shadow: 0 12px 24px rgba(var(--oj-color-primary-rgb) / 25%);
}

.btn-primary:hover {
    transform: translateY(-1px);
}

.btn-ghost {
    background: transparent;
    color: var(--promo-text);
    border: 1px solid var(--promo-border);
}

.hero-card {
    background: var(--promo-card);
    border: 1px solid var(--promo-border);
    border-radius: 20px;
    padding: 24px;
    display: grid;
    gap: 16px;
    min-height: 100%;
    box-shadow: inset 0 0 0 1px rgba(255, 255, 255, 0.02);
}

.hero-card-panel {
    padding: 16px;
    border-radius: 18px;
    background: color-mix(in srgb, var(--oj-bg-base) 68%, transparent);
    border: 1px solid var(--oj-border-light);
    box-shadow: inset 0 0 0 1px rgba(255, 255, 255, 0.02);
}

.hero-card-title {
    font-size: 18px;
    font-weight: 600;
}

. overview-stack {
    display: grid;
    gap: 12px;
}

.overview-item {
    padding: 14px 16px;
    border-radius: 14px;
    background: color-mix(in srgb, var(--oj-bg-base) 64%, transparent);
    border: 1px solid rgba(var(--oj-color-primary-rgb) / 18%);
    box-shadow: 0 10px 20px rgba(2, 6, 23, 0.25);
}

.overview-title {
    font-size: 14px;
    font-weight: 600;
    color: var(--promo-text);
}

.overview-desc {
    margin-top: 6px;
    font-size: 13px;
    color: var(--promo-text-muted);
    line-height: 1.6;
}

.overview-divider {
    height: 1px;
    margin: 14px 0;
    background: rgba(255, 255, 255, 0.06);
}

.overview-note {
    font-size: 13px;
    color: var(--promo-text-muted);
    line-height: 1.6;
}

@media (max-width: 640px) {
    .hero {
        grid-template-columns: 1fr;
    }
}

.section {
    display: grid;
    gap: 16px;
}

.section-title {
    font-size: 20px;
    font-weight: 600;
}

.grid,
.split {
    display: grid;
    gap: 16px;
    grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
}

.card {
    background: var(--promo-card);
    border: 1px solid var(--promo-border);
    border-radius: 18px;
    padding: 20px;
    box-shadow: 0 10px 30px rgba(2, 6, 23, 0.35);
}

.card-title {
    font-size: 16px;
    font-weight: 600;
}

.card-text {
    margin-top: 10px;
    font-size: 14px;
    line-height: 1.7;
    color: var(--promo-text-muted);
}

.list {
    display: grid;
    gap: 10px;
    margin: 0;
    padding-left: 18px;
    color: var(--promo-text-muted);
    font-size: 14px;
}

.cta {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 16px;
    background: linear-gradient(
        120deg,
        var(--oj-color-primary-soft),
        color-mix(in srgb, var(--oj-bg-light) 60%, transparent)
    );
    border: 1px solid var(--promo-border);
    border-radius: 20px;
    padding: 24px;
}

.cta-title {
    font-size: 20px;
    font-weight: 600;
}

.cta-subtitle {
    color: var(--promo-text-muted);
    font-size: 14px;
    margin-top: 6px;
}

@media (max-width: 768px) {
    .cta {
        flex-direction: column;
        align-items: flex-start;
    }
}

@media (prefers-reduced-motion: reduce) {
    .btn {
        transition: none;
    }
}
</style>
