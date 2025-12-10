/**
 * 系统设置状态管理模块
 *
 * @description 用于管理系统设置相关的状态和操作
 *
 * @module stores/modules/setting
 */
import { defineStore } from "pinia";
import { ref, computed } from "vue";
import type { ThemeMode } from "@/types/setting/them";

export const useSettingStore = defineStore(
    "settingStore",
    () => {
        // 主题模式状态：默认为深色主题
        const themeMode = ref<ThemeMode>("dark");

        /**
         * 计算当前实际生效的主题
         */
        const effectiveTheme = computed(() => {
            return themeMode.value === "system" ? getSystemTheme() : themeMode.value;
        });

        /**
         * 计算当前是否为深色主题
         */
        const isDark = computed(() => {
            return effectiveTheme.value === "dark";
        });

        /**
         * 获取系统偏好的主题
         */
        const getSystemTheme = (): "dark" | "light" => {
            if (typeof window === "undefined") return "dark";
            return window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";
        };

        /**
         * 设置主题模式
         */
        const setTheme = (mode: ThemeMode) => {
            themeMode.value = mode;
        };

        /**
         * 切换主题 (在 dark 和 light 之间切换)
         */
        const toggleTheme = () => {
            const newMode = isDark.value ? "light" : "dark";
            setTheme(newMode);
        };

        return {
            themeMode,
            effectiveTheme,
            isDark,
            setTheme,
            toggleTheme,
        };
    },
    {
        persist: { key: "setting", storage: localStorage },
    }
);
