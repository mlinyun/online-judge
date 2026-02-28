<script setup lang="ts">
/**
 * 管理后台 - 仪表盘页面
 *
 * @description 统计概览卡片 + 快捷导航入口
 */
defineOptions({ name: "AdminDashboard" });

const router = useRouter();

interface QuickNav {
    title: string;
    desc: string;
    icon: string;
    route: string;
    accent: string;
}

const quickNavs: QuickNav[] = [
    { title: "题目管理", desc: "查看和管理所有题目", icon: "document", route: "admin-problem", accent: "cyan" },
    { title: "题解管理", desc: "审核和管理题解内容", icon: "notebook", route: "admin-solution", accent: "emerald" },
    { title: "公告管理", desc: "发布和管理系统公告", icon: "bell", route: "admin-announcement", accent: "amber" },
    { title: "用户管理", desc: "管理所有注册用户", icon: "user", route: "admin-user", accent: "purple" },
    { title: "讨论管理", desc: "管理社区讨论帖子", icon: "chat-dot-round", route: "admin-discussion", accent: "blue" },
    { title: "评论管理", desc: "审核和管理评论", icon: "comment", route: "admin-comment", accent: "rose" },
];

const navigate = (routeName: string) => {
    router.push({ name: routeName });
};

/** 获取当前时间问候语 */
const greeting = computed(() => {
    const h = new Date().getHours();
    if (h < 6) return "夜深了 🌙";
    if (h < 12) return "早上好 ☀️";
    if (h < 18) return "下午好 🌤️";
    return "晚上好 🌇";
});
</script>

<template>
    <div class="admin-dashboard">
        <!-- 欢迎区域 -->
        <div class="welcome-section">
            <h1 class="welcome-title">{{ greeting }}，管理员</h1>
            <p class="welcome-desc">欢迎回到凌云在线编程评测平台管理后台</p>
        </div>

        <!-- 快捷导航 -->
        <h2 class="section-title">
            <el-icon><i-ep-grid /></el-icon>
            <span>快捷导航</span>
        </h2>
        <div class="nav-grid">
            <div
                v-for="nav in quickNavs"
                :key="nav.route"
                class="nav-card"
                :class="[`accent-${nav.accent}`]"
                @click="navigate(nav.route)"
            >
                <div class="nav-icon">
                    <el-icon :size="28">
                        <i-ep-document v-if="nav.icon === 'document'" />
                        <i-ep-notebook v-else-if="nav.icon === 'notebook'" />
                        <i-ep-bell v-else-if="nav.icon === 'bell'" />
                        <i-ep-user v-else-if="nav.icon === 'user'" />
                        <i-ep-chat-dot-round v-else-if="nav.icon === 'chat-dot-round'" />
                        <i-ep-comment v-else-if="nav.icon === 'comment'" />
                    </el-icon>
                </div>
                <div class="nav-content">
                    <h3 class="nav-title">{{ nav.title }}</h3>
                    <p class="nav-desc">{{ nav.desc }}</p>
                </div>
                <el-icon class="nav-arrow"><i-ep-arrow-right /></el-icon>
            </div>
        </div>

        <!-- 系统信息 -->
        <h2 class="section-title">
            <el-icon><i-ep-info-filled /></el-icon>
            <span>系统信息</span>
        </h2>
        <div class="info-grid">
            <div class="info-card">
                <span class="info-label">平台名称</span>
                <span class="info-value">凌云在线编程评测平台 (LYOJ)</span>
            </div>
            <div class="info-card">
                <span class="info-label">前端版本</span>
                <span class="info-value">v2.0.0</span>
            </div>
            <div class="info-card">
                <span class="info-label">技术栈</span>
                <span class="info-value">Vue 3 + TypeScript + Element Plus</span>
            </div>
            <div class="info-card">
                <span class="info-label">后端 API</span>
                <span class="info-value">C++ (cpp-httplib) + MongoDB + Redis</span>
            </div>
        </div>
    </div>
</template>

<style scoped>
.admin-dashboard {
    padding: var(--oj-spacing-4);
}

/* ── 欢迎 ── */
.welcome-section {
    padding: var(--oj-spacing-6);
    margin-bottom: var(--oj-spacing-6);
    background: linear-gradient(135deg, rgb(var(--oj-color-primary-rgb) / 15%), rgb(var(--oj-color-primary-rgb) / 5%));
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 20%);
    border-radius: var(--oj-radius-xl);
}

.welcome-title {
    margin: 0 0 var(--oj-spacing-2);
    font-size: var(--oj-font-size-2xl);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.welcome-desc {
    margin: 0;
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

/* ── 标题 ── */
.section-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin: 0 0 var(--oj-spacing-4);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

/* ── 快捷导航网格 ── */
.nav-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
    gap: var(--oj-spacing-4);
    margin-bottom: var(--oj-spacing-6);
}

.nav-card {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
    padding: var(--oj-spacing-5);
    cursor: pointer;
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-xl);
    backdrop-filter: blur(var(--oj-glass-blur));
    transition: all 0.25s ease;
}

.nav-card:hover {
    border-color: rgb(var(--oj-color-primary-rgb) / 40%);
    box-shadow: 0 4px 20px rgb(var(--oj-color-primary-rgb) / 15%);
    transform: translateY(-2px);
}

.nav-icon {
    display: flex;
    flex-shrink: 0;
    align-items: center;
    justify-content: center;
    width: 48px;
    height: 48px;
    border-radius: var(--oj-radius-lg);
}

.accent-cyan .nav-icon {
    color: rgb(6 182 212);
    background: rgb(6 182 212 / 12%);
}

.accent-emerald .nav-icon {
    color: rgb(52 211 153);
    background: rgb(52 211 153 / 12%);
}

.accent-amber .nav-icon {
    color: rgb(245 158 11);
    background: rgb(245 158 11 / 12%);
}

.accent-purple .nav-icon {
    color: rgb(168 85 247);
    background: rgb(168 85 247 / 12%);
}

.accent-blue .nav-icon {
    color: rgb(96 165 250);
    background: rgb(96 165 250 / 12%);
}

.accent-rose .nav-icon {
    color: rgb(251 113 133);
    background: rgb(251 113 133 / 12%);
}

.nav-content {
    flex: 1;
    min-width: 0;
}

.nav-title {
    margin: 0;
    font-size: var(--oj-font-size-base);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.nav-desc {
    margin: var(--oj-spacing-1) 0 0;
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.nav-arrow {
    flex-shrink: 0;
    color: var(--oj-text-color-muted);
    transition: transform 0.2s ease;
}

.nav-card:hover .nav-arrow {
    color: var(--oj-color-primary);
    transform: translateX(4px);
}

/* ── 系统信息 ── */
.info-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
    gap: var(--oj-spacing-3);
}

.info-card {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-1);
    padding: var(--oj-spacing-4);
    background: var(--oj-card-bg);
    border: 1px solid var(--oj-card-border);
    border-radius: var(--oj-radius-lg);
}

.info-label {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.info-value {
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color);
}
</style>
