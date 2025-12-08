/**
 * 系统设置状态管理模块
 *
 * @description 用于管理系统设置相关的状态和操作
 *
 * @module stores/modules/setting
 */
import { defineStore } from "pinia";

export const useSettingStore = defineStore(
    "settingStore",
    () => {
        // 是否启用暗黑模式
        const isDarkMode = ref<boolean>(false);

        // 计算属性：获取暗黑模式状态
        const getIsDarkMode = computed(() => isDarkMode.value);

        /**
         * 切换暗黑模式状态
         */
        const toggleDarkMode = () => {
            isDarkMode.value = !isDarkMode.value;
        };

        return {
            // 状态
            isDarkMode,
            // 计算属性
            getIsDarkMode,
            // 方法
            toggleDarkMode,
        };
    },
    {
        persist: { key: "setting", storage: localStorage },
    }
);
