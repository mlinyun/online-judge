<script setup lang="ts">
defineOptions({ name: "HelpPage" });

const quickStart = [
    "创建账号并完善个人资料",
    "在题库选择题目并提交代码",
    "查看测评详情与测试点结果",
    "在讨论区获取同伴反馈",
];

const supportCards = [
    {
        title: "知识库",
        desc: "从入门指南到竞赛配置，覆盖常用流程。",
    },
    {
        title: "技术支持",
        desc: "遇到异常评测或账号问题，我们提供专人响应。",
    },
    {
        title: "反馈通道",
        desc: "欢迎提交需求与建议，一起完善 LYOJ。",
    },
];

const faq = [
    {
        q: "为什么一直显示 Pending？",
        a: "评测任务排队中，请稍后刷新；如超过 5 分钟仍无结果，请提供记录 ID。",
    },
    {
        q: "如何查看提交详情？",
        a: "在“测评状态”或“我的提交”列表进入详情页，可查看编译日志与测试点结果。",
    },
    {
        q: "支持哪些语言？",
        a: "支持 C/C++、Java、Python、Go、JavaScript 等，具体以题目配置为准。",
    },
];
</script>

<template>
    <section class="promo-page help-page" aria-label="帮助中心">
        <header class="hero">
            <div class="hero-content">
                <div class="eyebrow">Support</div>
                <h1 class="hero-title">帮助中心</h1>
                <p class="hero-subtitle">快速定位问题、找到答案、继续专注于你的提交。</p>
                <div class="hero-actions">
                    <router-link to="/problem/list" class="btn btn-primary">开始练习</router-link>
                    <a class="btn btn-ghost" href="mailto:support@lyoj.com">联系支持</a>
                </div>
            </div>
            <div class="hero-card">
                <div class="hero-card-title">快速开始</div>
                <ol class="list">
                    <li v-for="item in quickStart" :key="item">{{ item }}</li>
                </ol>
            </div>
        </header>

        <section class="section">
            <div class="section-title">支持资源</div>
            <div class="grid">
                <div v-for="item in supportCards" :key="item.title" class="card">
                    <div class="card-title">{{ item.title }}</div>
                    <p class="card-text">{{ item.desc }}</p>
                </div>
            </div>
        </section>

        <section class="section">
            <div class="section-title">常见问题</div>
            <div class="grid">
                <div v-for="item in faq" :key="item.q" class="card">
                    <div class="card-title">{{ item.q }}</div>
                    <p class="card-text">{{ item.a }}</p>
                </div>
            </div>
        </section>

        <section class="cta">
            <div>
                <div class="cta-title">仍未解决？</div>
                <div class="cta-subtitle">发送记录 ID 与截图，我们将在 24 小时内响应。</div>
            </div>
            <a class="btn btn-primary" href="mailto:support@lyoj.com">提交问题</a>
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
    grid-template-columns: repeat(auto-fit, minmax(260px, 1fr));
    padding: clamp(24px, 4vw, 40px);
    border-radius: var(--oj-radius-2xl);
    background: color-mix(in srgb, var(--oj-bg-base) 70%, transparent);
    border: 1px solid var(--promo-border);
    position: relative;
    overflow: hidden;
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
}

.hero-card-title {
    font-size: 18px;
    font-weight: 600;
}

.section {
    display: grid;
    gap: 16px;
}

.section-title {
    font-size: 20px;
    font-weight: 600;
}

.grid {
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
