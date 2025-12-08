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
        return {};
    },
    {
        persist: { key: "setting", storage: localStorage },
    }
);
