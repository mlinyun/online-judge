<script setup lang="ts">
/**
 * 主布局顶部导航栏组件
 * 包含 Logo、导航菜单、用户信息、主题切换
 */
import { ref, computed } from "vue";
import { useRouter, useRoute } from "vue-router";
import { Close as IconEpClose, Menu as IconEpMenu } from "@element-plus/icons-vue";
import ThemeToggle from "./components/them-toggle.vue";
import UserDropdown from "./components/user-dropdown.vue";
import LogoSvg from "@/assets/logo.svg?component";

defineOptions({ name: "MainHeader" });

const router = useRouter();
const route = useRoute();

// 移动端菜单状态
const isMobileMenuOpen = ref(false);

// 导航菜单项
const navItems = [
    { name: "首页", path: "/", icon: IconEpHouse },
    { name: "题库", path: "/problem/list", icon: IconEpCollection },
    { name: "讨论", path: "/discussion/list", icon: IconEpChatDotRound },
    { name: "题解", path: "/solution/list", icon: IconEpReading },
    { name: "测评", path: "/status-record", icon: IconEpDocument },
    { name: "排名", path: "/user/rank", icon: IconEpTrophy },
];

// 计算当前激活的菜单项
const activeMenu = computed(() => {
    for (const item of navItems) {
        if (item.path === "/" && route.path === "/") {
            return item.path;
        }
        if (item.path !== "/" && route.path.startsWith(item.path)) {
            return item.path;
        }
    }
    return "/";
});

// 导航到指定路径
const handleNavigation = (path: string) => {
    router.push(path);
    isMobileMenuOpen.value = false;
};

// 切换移动端菜单
const toggleMobileMenu = () => {
    isMobileMenuOpen.value = !isMobileMenuOpen.value;
};
</script>

<template>
    <nav class="oj-navbar">
        <div class="oj-navbar-container">
            <!-- Logo -->
            <div class="oj-navbar-brand" @click="handleNavigation('/')">
                <LogoSvg class="oj-navbar-brand-logo" />
                <span class="oj-navbar-brand-text"> LY<span class="oj-navbar-brand-accent">OJ</span> </span>
            </div>

            <!-- 桌面端导航菜单 -->
            <el-menu class="oj-navbar-menu" :default-active="activeMenu" mode="horizontal" @select="handleNavigation">
                <el-menu-item v-for="item in navItems" :key="item.path" :index="item.path">
                    <el-icon>
                        <component :is="item.icon" />
                    </el-icon>
                    <span>{{ item.name }}</span>
                </el-menu-item>
            </el-menu>

            <!-- 右侧工具栏 -->
            <div class="oj-navbar-actions">
                <!-- 主题切换 -->
                <ThemeToggle />

                <!-- 用户信息 -->
                <UserDropdown />

                <!-- 移动端菜单按钮 -->
                <button class="oj-mobile-menu-btn" @click="toggleMobileMenu">
                    <el-icon :size="20">
                        <component :is="isMobileMenuOpen ? IconEpClose : IconEpMenu" />
                    </el-icon>
                </button>
            </div>
        </div>

        <!-- 移动端菜单 -->
        <Transition name="oj-slide-down">
            <div v-if="isMobileMenuOpen" class="oj-mobile-menu">
                <a
                    v-for="item in navItems"
                    :key="item.path"
                    class="oj-mobile-nav-link"
                    :class="{ active: activeMenu === item.path }"
                    @click="handleNavigation(item.path)"
                >
                    <el-icon>
                        <component :is="item.icon" />
                    </el-icon>
                    <span>{{ item.name }}</span>
                </a>
            </div>
        </Transition>
    </nav>
</template>

<style scoped>
/* ============================================
 * 导航栏整体样式
 * ============================================ */
.oj-navbar {
    position: fixed;
    top: 0;
    right: 0;
    left: 0;
    z-index: var(--oj-z-navbar);
    height: var(--oj-navbar-height);
    border-bottom: 1px solid var(--oj-glass-border);
    background: var(--oj-glass-bg);
    backdrop-filter: blur(12px);
    animation: oj-navbar-appear 0.5s ease-out;
}

@keyframes oj-navbar-appear {
    from {
        opacity: 0;
        transform: translateY(-100%);
    }

    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.oj-navbar-container {
    display: flex;
    align-items: center;
    justify-content: space-between;
    max-width: var(--oj-container-xl);
    height: 100%;
    margin-right: auto;
    margin-left: auto;
    padding-right: var(--oj-spacing-4);
    padding-left: var(--oj-spacing-4);
}

/* ============================================
 * Logo 样式
 * ============================================ */

.oj-navbar-brand {
    display: flex;
    flex-shrink: 0;
    align-items: center;
    gap: var(--oj-spacing-3);
    cursor: pointer;
    transition: transform 0.2s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.oj-navbar-brand:hover {
    transform: scale(1.05);
}

.oj-navbar-brand-logo {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    animation: oj-pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite;
}

.oj-navbar-brand-text {
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xl);
    font-weight: 700;
    color: var(--oj-text-color);
    letter-spacing: 0.05em;
}

.oj-navbar-brand-accent {
    color: var(--oj-color-primary);
}

/* ============================================
 * 桌面端菜单样式
 * ============================================ */

.oj-navbar-menu {
    display: none;
    flex: 1;
    margin-left: var(--oj-spacing-10);
    border-bottom: none;
    background: transparent;
}

@media (width >=768px) {
    .oj-navbar-menu {
        display: flex;
        justify-content: center;
    }
}

/* 菜单项样式 */
.oj-navbar-menu :deep(.el-menu-item) {
    position: relative;
    height: var(--oj-navbar-height);
    padding: 0 var(--oj-spacing-3);
    border-bottom: 2px solid transparent;
    font-size: var(--oj-font-size-base);
    font-weight: 500;
    color: var(--oj-text-color-secondary);
    line-height: var(--oj-navbar-height);
    cursor: pointer;
    background: transparent !important;
    transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
}

/* 菜单项悬停效果 */
.oj-navbar-menu :deep(.el-menu-item:hover) {
    color: var(--oj-color-primary) !important;
    background: transparent !important;
}

.oj-navbar-menu :deep(.el-menu-item:hover::before) {
    position: absolute;
    bottom: -1px;
    left: 50%;
    width: 0;
    height: 2px;
    content: "";
    background: linear-gradient(90deg, transparent 0%, var(--oj-color-primary) 50%, transparent 100%);
    animation: oj-menu-underline 0.4s ease-out;
}

@keyframes oj-menu-underline {
    from {
        left: 50%;
        width: 0;
    }

    to {
        left: 0;
        width: 100%;
    }
}

/* 菜单项激活效果 */
.oj-navbar-menu :deep(.el-menu-item.is-active) {
    border-bottom-color: var(--oj-color-primary) !important;
    color: var(--oj-color-primary) !important;
    background: transparent !important;
}

.oj-navbar-menu :deep(.el-menu-item.is-active)::after {
    position: absolute;
    bottom: -1px;
    left: 0;
    right: 0;
    height: 2px;
    content: "";
    background: linear-gradient(90deg, var(--oj-color-primary), var(--oj-color-primary-light));
    box-shadow: 0 0 8px var(--oj-color-primary);
    animation: oj-active-glow 0.5s ease-out;
}

@keyframes oj-active-glow {
    from {
        box-shadow: 0 0 0 var(--oj-color-primary);
    }

    to {
        box-shadow: 0 0 8px var(--oj-color-primary);
    }
}

/* 隐藏桌面端菜单项的图标 */
.oj-navbar-menu :deep(.el-icon) {
    display: none !important;
}

/* ============================================
 * 右侧工具栏
 * ============================================ */

.oj-navbar-actions {
    display: flex;
    align-items: center;
    gap: var(--oj-spacing-3);
}

/* ============================================
 * 移动端菜单按钮
 * ============================================ */

.oj-mobile-menu-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 40px;
    height: 40px;
    border: none;
    border-radius: var(--oj-radius-md);
    background: var(--oj-bg-tertiary);
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.oj-mobile-menu-btn:hover {
    background: var(--oj-bg-hover);
    color: var(--oj-color-primary);
    transform: rotate(90deg);
}

@media (width >=768px) {
    .oj-mobile-menu-btn {
        display: none;
    }
}

/* ============================================
 * 移动端菜单
 * ============================================ */

.oj-mobile-menu {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-1);
    padding: var(--oj-spacing-4);
    border-bottom: 1px solid var(--oj-glass-border);
    background: var(--oj-glass-bg);
    backdrop-filter: blur(12px);
}

@media (width >=768px) {
    .oj-mobile-menu {
        display: none;
    }
}

.oj-mobile-nav-link {
    position: relative;
    display: flex;
    align-items: center;
    gap: var(--oj-spacing-2);
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    border-radius: var(--oj-radius-md);
    font-size: var(--oj-font-size-sm);
    font-weight: 500;
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: transparent;
    overflow: hidden;
    transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
}

/* 移动端菜单项悬停效果 */
.oj-mobile-nav-link:hover {
    background: var(--oj-bg-hover);
    color: var(--oj-color-primary);
    transform: translateX(4px);
}

.oj-mobile-nav-link:hover::before {
    position: absolute;
    top: 0;
    left: 0;
    bottom: 0;
    width: 2px;
    content: "";
    background: linear-gradient(180deg, var(--oj-color-primary), var(--oj-color-primary-light));
    animation: oj-slide-in 0.3s ease-out;
}

@keyframes oj-slide-in {
    from {
        top: 50%;
        height: 0;
    }

    to {
        top: 0;
        height: 100%;
    }
}

/* 移动端菜单项激活效果 */
.oj-mobile-nav-link.active {
    background: rgb(var(--oj-color-primary-rgb) / 10%);
    color: var(--oj-color-primary);
}

.oj-mobile-nav-link.active::before {
    position: absolute;
    top: 0;
    left: 0;
    bottom: 0;
    width: 2px;
    content: "";
    background: linear-gradient(180deg, var(--oj-color-primary), var(--oj-color-primary-light));
    box-shadow: 0 0 8px var(--oj-color-primary);
}

/* 移动端菜单项图标样式 */
.oj-mobile-nav-link :deep(.el-icon) {
    display: flex;
    align-items: center;
    font-size: var(--oj-font-size-lg);
    transition: transform 0.3s ease;
}

.oj-mobile-nav-link:hover :deep(.el-icon) {
    transform: scale(1.2);
}

/* ============================================
 * 过渡动画
 * ============================================ */

.oj-slide-down-enter-active,
.oj-slide-down-leave-active {
    transition: all 0.3s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.oj-slide-down-enter-from,
.oj-slide-down-leave-to {
    opacity: 0;
    transform: translateY(-10px);
}

.oj-slide-down-enter-to,
.oj-slide-down-leave-from {
    opacity: 1;
    transform: translateY(0);
}
</style>
