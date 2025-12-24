<script setup lang="ts">
/**
 * 管理后台 - 侧边栏
 */
import { computed, ref, type Component } from "vue";
import {
    Bell as IconEpBell,
    ChatDotRound as IconEpChatDotRound,
    Comment as IconEpComment,
    Document as IconEpDocument,
    Expand as IconEpExpand,
    Odometer as IconEpOdometer,
    Platform as IconEpPlatform,
    Reading as IconEpReading,
    Setting as IconEpSetting,
    User as IconEpUser,
} from "@element-plus/icons-vue";
import { useRoute, useRouter } from "vue-router";

defineOptions({ name: "AdminAside" });

// 定义 Props
const props = defineProps<{
    collapsed: boolean;
}>();

// 定义 Emits
const emit = defineEmits<{
    (e: "update:collapsed", value: boolean): void;
}>();

const router = useRouter();
const route = useRoute();

const isLogoHovered = ref(false);

// 侧边栏宽度
const sidebarWidth = computed(() => (props.collapsed ? "64px" : "224px"));

// 切换侧边栏折叠状态
const toggleSidebar = () => {
    emit("update:collapsed", !props.collapsed);
};

// 导航到指定路径
const navigateTo = (path: string) => {
    router.push(path);
};

// 处理 Logo 点击
const handleLogoClick = () => {
    if (props.collapsed) {
        toggleSidebar();
    } else {
        navigateTo("/admin");
    }
};

// 菜单项类型
interface MenuItem {
    name: string;
    path?: string;
    icon?: Component;
    children?: MenuItem[];
}

// 导航菜单项
const menuItems: MenuItem[] = [
    {
        name: "仪表盘",
        children: [{ name: "首页", path: "/admin/dashboard", icon: IconEpOdometer }],
    },
    {
        name: "内容管理",
        children: [
            { name: "题目管理", path: "/admin/problem", icon: IconEpDocument },
            { name: "题解管理", path: "/admin/solution", icon: IconEpReading },
            { name: "公告管理", path: "/admin/announcement", icon: IconEpBell },
        ],
    },
    {
        name: "社区管理",
        children: [
            { name: "用户管理", path: "/admin/user", icon: IconEpUser },
            { name: "讨论管理", path: "/admin/discussion", icon: IconEpChatDotRound },
            { name: "评论管理", path: "/admin/comment", icon: IconEpComment },
        ],
    },
    {
        name: "系统设置",
        children: [{ name: "系统设置", path: "/admin/setting", icon: IconEpSetting }],
    },
];

// 返回前台
const goToFrontend = () => {
    router.push("/");
};
</script>

<template>
    <!-- 侧边栏 -->
    <aside class="admin-sidebar" :style="{ width: sidebarWidth }">
        <!-- Logo -->
        <div class="sidebar-header">
            <div
                class="sidebar-brand"
                @click="handleLogoClick"
                @mouseenter="isLogoHovered = true"
                @mouseleave="isLogoHovered = false"
            >
                <el-icon class="brand-icon" :size="22">
                    <component :is="props.collapsed && isLogoHovered ? IconEpExpand : IconEpPlatform" />
                </el-icon>
                <Transition name="fade">
                    <span v-if="!props.collapsed" class="brand-text">
                        LY<span class="brand-accent">OJ</span>
                        <span class="brand-suffix">Admin</span>
                    </span>
                </Transition>
            </div>
            <!-- 折叠按钮 (仅展开时显示) -->
            <div v-if="!props.collapsed" class="sidebar-toggle" @click="toggleSidebar">
                <el-icon :size="22" class="toggle-icon">
                    <i-ep-fold />
                </el-icon>
            </div>
        </div>

        <!-- 导航菜单 -->
        <el-menu class="sidebar-menu" :collapse="props.collapsed" :default-active="route.path" router>
            <template v-for="item in menuItems" :key="item.name">
                <el-menu-item-group :title="item.name">
                    <el-menu-item v-for="child in item.children" :key="child.path" :index="child.path">
                        <el-icon>
                            <component :is="child.icon" />
                        </el-icon>
                        <template #title>
                            <span>{{ child.name }}</span>
                        </template>
                    </el-menu-item>
                </el-menu-item-group>
            </template>
        </el-menu>

        <!-- 底部返回前台 -->
        <div class="sidebar-footer">
            <a class="sidebar-footer-btn" :title="props.collapsed ? '返回前台' : ''" @click="goToFrontend">
                <el-icon class="nav-icon" :size="18">
                    <i-ep-back />
                </el-icon>
                <Transition name="fade">
                    <span v-if="!props.collapsed" class="nav-text">返回前台</span>
                </Transition>
            </a>
        </div>
    </aside>
</template>

<style scoped>
/* 侧边栏 */
.admin-sidebar {
    position: fixed;
    top: 0;
    bottom: 0;
    left: 0;
    z-index: var(--oj-z-sidebar);
    display: flex;
    flex-direction: column;
    background: var(--oj-glass-bg);
    border-right: 1px solid var(--oj-glass-border);
    backdrop-filter: blur(12px);
    transition: width 0.3s ease;
}

/* Logo */
.sidebar-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    height: var(--oj-navbar-height);
    padding: 0 var(--oj-spacing-4);
    border-bottom: 1px solid var(--oj-border-color);
}

.sidebar-brand {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    overflow: hidden;
    cursor: pointer;
}

.brand-icon {
    flex-shrink: 0;
    color: var(--oj-color-primary);
    transition: transform 0.2s;
}

.sidebar-brand:hover .brand-icon {
    transform: scale(1.1);
}

.brand-text {
    display: flex;
    gap: var(--oj-spacing-1);
    align-items: baseline;
    font-family: var(--oj-font-family-mono), monospace;
    font-size: var(--oj-font-size-xl);
    font-weight: 700;
    white-space: nowrap;
}

.brand-accent {
    color: var(--oj-color-primary);
}

.brand-suffix {
    font-size: var(--oj-font-size-base);
    font-weight: 400;
    color: var(--oj-text-color-muted);
}

.sidebar-toggle {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 24px;
    height: 24px;
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    border-radius: var(--oj-radius-sm);
    transition: all 0.2s;
}

.sidebar-toggle:hover {
    color: var(--oj-text-color);
    background: var(--oj-bg-hover);
}

.toggle-icon {
    flex-shrink: 0;
    color: var(--oj-color-primary);
    transition: transform 0.2s;
}

/* 导航菜单 */
.sidebar-menu {
    flex: 1;
    overflow-y: auto;
    background: transparent !important;
    border-right: none !important;
}

/* 折叠状态下隐藏分组标题 */
.sidebar-menu.el-menu--collapse :deep(.el-menu-item-group__title) {
    display: none;
}

/** 分组标题样式 */
.sidebar-menu :deep(.el-menu-item-group__title) {
    padding: var(--oj-spacing-3) var(--oj-spacing-3) var(--oj-spacing-2) !important;
    font-size: var(--oj-font-size-xs);
    font-weight: 700;
    color: var(--oj-text-muted);
    text-transform: uppercase;
    letter-spacing: 0.05em;
}

/** 菜单项样式 */
.sidebar-menu :deep(.el-menu-item) {
    height: 48px;
    margin: 0 var(--oj-spacing-2) var(--oj-spacing-1);
    color: var(--oj-text-color-secondary);
    border-radius: var(--oj-radius-lg);
}

/* 折叠状态下调整菜单项样式以居中图标 */
.sidebar-menu.el-menu--collapse :deep(.el-menu-item) {
    display: flex;
    align-items: center;
    justify-content: center;
    padding: 0;
    margin: var(--oj-spacing-5) 0;
}

.sidebar-menu :deep(.el-menu-item:hover) {
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
}

.sidebar-menu :deep(.el-menu-item.is-active) {
    font-weight: 500;
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
}

.sidebar-menu :deep(.el-icon) {
    width: 24px;
    font-size: 18px;
    vertical-align: middle;
    text-align: center;
}

/* 底部返回前台 */
.sidebar-footer {
    padding: var(--oj-spacing-3);
    border-top: 1px solid var(--oj-border-color);
}

.sidebar-footer-btn {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    border-radius: var(--oj-radius-lg);
    transition: all 0.2s;
}

.sidebar-footer-btn:hover {
    color: var(--oj-color-danger);
    background: rgb(var(--oj-color-danger-rgb) / 10%);
}

/* 折叠状态下调整底部按钮样式以居中图标 */
.admin-sidebar[style*="width: 64px"] .sidebar-footer-btn {
    justify-content: center;
    padding: var(--oj-spacing-3) 0;
}

.nav-icon {
    flex-shrink: 0;
}

.nav-text {
    white-space: nowrap;
}

/* 过渡动画 */
.fade-enter-active,
.fade-leave-active {
    transition: opacity 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
    opacity: 0;
}
</style>
