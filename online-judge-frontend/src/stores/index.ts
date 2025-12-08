/**
 * Pinia Store 配置模块
 *
 * 提供全局状态管理的初始化和配置
 *
 * ## 主要功能
 *
 * - Pinia Store 实例创建
 * - 持久化插件配置（pinia-plugin-persistedstate）
 * - 版本化存储键管理
 * - 自动数据迁移（跨版本）
 * - LocalStorage 序列化配置
 * - Store 初始化函数
 *
 * ## 持久化策略
 *
 * - 使用 StorageKeyManager 生成版本化的存储键
 * - 格式：sys-v{version}-{storeId}
 * - 自动迁移旧版本数据到当前版本
 * - 使用 localStorage 作为存储介质
 */
import type { App } from "vue";
import { createPinia } from "pinia";
import { createPersistedState } from "pinia-plugin-persistedstate";
import { StorageKeyManager } from "@/utils/storage/storage-key-manager";

// 创建存储键名管理器实例
const storageKeyManager = new StorageKeyManager();

// 创建 Pinia Store 实例
export const piniaStore = createPinia();

// 配置持久化插件
piniaStore.use(
    createPersistedState({
        // 自定义存储键名生成逻辑
        key: (storeId: string) => storageKeyManager.getStorageKey(storeId),
        // 使用 localStorage 作为存储介质
        storage: localStorage,
        // 可选：自定义序列化和反序列化方法
        serializer: {
            serialize: (data) => JSON.stringify(data),
            deserialize: (data) => JSON.parse(data),
        },
    })
);

/**
 * 初始化 Pinia Store
 * @param app Vue 应用实例
 */
export function initPiniaStore(app: App<Element>): void {
    app.use(piniaStore);
}
