<script setup lang="ts">
defineOptions({ name: "TermsPage" });

const sections = [
    {
        title: "账户与使用",
        items: ["注册信息需真实、准确", "妥善保管账号与凭证", "不得滥用平台资源"],
    },
    {
        title: "内容与版权",
        items: ["遵守法律法规与社区规范", "平台内容与标识受保护"],
    },
    {
        title: "评测与结果",
        items: ["评测结果由自动系统生成", "系统持续优化但不对误差担责"],
    },
    {
        title: "服务调整",
        items: ["可基于运营需要调整功能", "重要变更将通过公告告知"],
    },
];
</script>

<template>
    <section class="promo-page terms-page" aria-label="服务条款">
        <header class="hero">
            <div class="hero-content">
                <div class="eyebrow">Terms</div>
                <h1 class="hero-title">服务条款</h1>
                <p class="hero-subtitle">条款清晰透明，帮助你安心使用平台服务。</p>
                <div class="hero-actions">
                    <router-link class="btn btn-primary" to="/auth/register">同意并加入</router-link>
                    <a class="btn btn-ghost" href="mailto:legal@lyoj.com">联系法务</a>
                </div>
            </div>
            <div class="hero-card">
                <div class="hero-card-title">条款说明</div>
                <p class="card-text">
                    本条款适用于你使用 LYOJ 平台的全部服务。继续使用即表示你已阅读并同意遵守这些条款。
                </p>
            </div>
        </header>

        <section class="section">
            <div class="section-title">条款要点</div>
            <div class="grid">
                <div v-for="section in sections" :key="section.title" class="card">
                    <div class="card-title">{{ section.title }}</div>
                    <ul class="list">
                        <li v-for="item in section.items" :key="item">{{ item }}</li>
                    </ul>
                </div>
            </div>
        </section>

        <section class="cta">
            <div>
                <div class="cta-title">还有疑问？</div>
                <div class="cta-subtitle">发邮件至 legal@lyoj.com 获取帮助。</div>
            </div>
            <a class="btn btn-primary" href="mailto:legal@lyoj.com">咨询条款</a>
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
    margin: 10px 0 0;
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
