<script setup lang="ts">
/**
 * 主布局页脚组件
 * 显示版权信息和相关链接
 */
defineOptions({ name: "MainFooter" });

/**
 * 获取当前年份
 */
const currentYear = new Date().getFullYear();

const links = [
    { name: "关于我们", href: "/about" },
    { name: "帮助中心", href: "/help" },
    { name: "隐私政策", href: "/privacy" },
    { name: "服务条款", href: "/terms" },
];

// const socialLinks = [
//     { name: "GitHub", icon: "fa-github", href: "https://github.com" },
// ];
</script>

<template>
    <footer class="app-footer">
        <div class="footer-container">
            <div class="footer-content">
                <!-- Logo 和描述 -->
                <div class="footer-brand">
                    <div class="brand-logo">
                        <span class="brand-text"> LY<span class="brand-accent">OJ</span> </span>
                    </div>
                    <p class="brand-desc">新一代在线评测系统，助力编程学习与竞赛训练</p>
                </div>

                <!-- 链接 -->
                <div class="footer-links">
                    <a v-for="link in links" :key="link.name" :href="link.href" class="footer-link">
                        {{ link.name }}
                    </a>
                </div>
            </div>

            <!-- 分隔线 -->
            <div class="footer-divider"></div>

            <!-- 版权信息 -->
            <div class="footer-bottom">
                <p class="copyright">© {{ currentYear }} LYOJ. All rights reserved.</p>
                <p class="powered-by">
                    Powered by
                    <a href="https://vuejs.org" target="_blank" rel="noopener" class="tech-link">Vue 3</a>
                    &
                    <a href="https://element-plus.org" target="_blank" rel="noopener" class="tech-link">
                        Element Plus
                    </a>
                </p>
            </div>
        </div>
    </footer>
</template>

<style scoped>
.app-footer {
    padding: var(--oj-spacing-4) 0 var(--oj-spacing-4);
    margin-top: auto;
    background: var(--oj-bg-secondary);
    border-top: 1px solid var(--oj-border-color);
}

.footer-container {
    max-width: var(--oj-container-xl);
    padding-right: var(--oj-spacing-4);
    padding-left: var(--oj-spacing-4);
    margin-right: auto;
    margin-left: auto;
}

.footer-content {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    align-items: center;
    justify-content: space-between;
}

@media (width >=768px) {
    .footer-content {
        flex-direction: row;
        align-items: center;
    }
}

/* Logo 和描述 */
.footer-brand {
    text-align: center;
}

@media (width >=768px) {
    .footer-brand {
        text-align: left;
    }
}

.brand-logo {
    margin-bottom: var(--oj-spacing-2);
}

.brand-text {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xl);
    font-weight: 700;
    color: var(--oj-text-color);
    letter-spacing: 0.05em;
}

.brand-accent {
    color: var(--oj-color-primary);
}

.brand-desc {
    max-width: 300px;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
}

/* 链接 */
.footer-links {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    justify-content: center;
}

@media (width >=768px) {
    .footer-links {
        gap: var(--oj-spacing-6);
    }
}

.footer-link {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
    text-decoration: none;
    transition: color 0.2s;
}

.footer-link:hover {
    color: var(--oj-color-primary);
}

/* 分隔线 */
.footer-divider {
    height: 1px;
    margin: var(--oj-spacing-4) 0;
    background: var(--oj-border-color);
}

/* 底部版权 */
.footer-bottom {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
    align-items: center;
    justify-content: space-between;
}

@media (width >=768px) {
    .footer-bottom {
        flex-direction: row;
    }
}

.copyright {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-muted);
}

.powered-by {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-muted);
}

.tech-link {
    color: var(--oj-color-primary);
    text-decoration: none;
    transition: color 0.2s;
}

.tech-link:hover {
    color: var(--oj-color-primary-light);
}
</style>
