/**
 * 应用配置常量
 *
 * 提供应用级别的配置常量
 *
 * ## 主要功能
 *
 * - 存储键名定义
 * - 应用配置项
 * - 默认值设置
 *
 * @module constants/config
 */

/**
 * 本地存储键名
 */
export const STORAGE_KEYS = {
    /** Token 键名 */
    TOKEN: "token",
    /** 用户信息键名 */
    USER_INFO: "user_info",
    /** 语言设置键名 */
    LANGUAGE: "language",
    /** 主题设置键名 */
    THEME: "theme",
    /** 侧边栏折叠状态键名 */
    SIDEBAR_COLLAPSED: "admin_sidebar_collapsed",
} as const;

/**
 * 应用默认配置
 */
export const APP_CONFIG = {
    /** 默认语言 */
    DEFAULT_LANGUAGE: "zh-CN",
    /** 默认主题 */
    DEFAULT_THEME: "light",
    /** 分页默认每页数量 */
    DEFAULT_PAGE_SIZE: 20,
    /** 分页可选每页数量 */
    PAGE_SIZE_OPTIONS: [5, 10, 20, 50, 100],
} as const;

/**
 * API 配置
 */
export const API_CONFIG = {
    /** API 基础路径 */
    BASE_PATH: "/api",
    /** API 版本 */
    VERSION: "v1",
} as const;
