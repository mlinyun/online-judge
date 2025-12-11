<script setup lang="ts">
/**
 * 主题切换组件
 * 支持深色/浅色/系统自动三种模式
 */
import { useTheme } from "@/composables/useTheme";
import { Sunny, Moon, Monitor } from "@element-plus/icons-vue";

defineOptions({ name: "ThemeToggle" });

const { themeMode, isDark, setTheme } = useTheme();

// 主题选项
const themeOptions = [
    { value: "light", label: "浅色", icon: Sunny },
    { value: "dark", label: "深色", icon: Moon },
    { value: "system", label: "跟随系统", icon: Monitor },
] as const;
</script>

<template>
    <el-dropdown trigger="click" @command="setTheme">
        <el-button class="theme-toggle-btn" :title="isDark ? '深色模式' : '浅色模式'">
            <el-icon :size="18">
                <component :is="isDark ? Moon : Sunny" />
            </el-icon>
        </el-button>

        <template #dropdown>
            <el-dropdown-menu>
                <el-dropdown-item
                    v-for="option in themeOptions"
                    :key="option.value"
                    :command="option.value"
                    :class="{ 'is-active': themeMode === option.value }"
                >
                    <el-icon class="dropdown-icon">
                        <component :is="option.icon" />
                    </el-icon>
                    <span>{{ option.label }}</span>
                </el-dropdown-item>
            </el-dropdown-menu>
        </template>
    </el-dropdown>
</template>

<style scoped>
.theme-toggle-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 36px;
    height: 36px;
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: transparent;
    border: none;
    border-radius: var(--oj-radius-md);
    transition: all 0.2s;
}

.theme-toggle-btn:hover {
    color: var(--oj-color-primary);
    background: var(--oj-bg-hover);
}

.dropdown-icon {
    margin-right: var(--oj-spacing-2);
}

:deep(.el-dropdown-menu__item.is-active) {
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 10%);
}

:deep(.el-dropdown-menu__item:not(.is-active):focus, .el-dropdown-menu__item:not(.is-active):hover) {
    color: var(--oj-color-primary);
    background-color: rgb(var(--oj-color-primary-rgb) / 10%);
}
</style>
