/**
 * HTTP 请求封装模块
 *
 * 基于 Axios 封装的 HTTP 请求工具，提供统一的请求/响应处理、错误处理、消息提示等功能
 *
 * ## 主要功能
 *
 * - 统一的请求/响应拦截
 * - 自动 Token 管理
 * - 大整数精度处理（int64）
 * - 统一错误处理与消息提示
 * - 完善的类型支持
 *
 * ## 使用场景
 *
 * - API 请求封装
 * - 统一响应处理
 * - 错误处理与提示
 *
 * @module utils/http
 */

import axios, {
    type AxiosInstance,
    type AxiosRequestConfig,
    type AxiosResponse,
    type AxiosError,
    type InternalAxiosRequestConfig,
} from "axios";
import JSONBig from "json-bigint";
// 注意：自动导入时不要显示引入 ElMessage，否则消息提示可能无法正常显示
// import { ElMessage } from "element-plus";
import type { ApiResponse } from "@/types/common/response.ts";
import {
    HttpStatusCode,
    HTTP_ERROR_MESSAGES,
    BusinessErrorCode,
    isAuthError,
    REQUEST_TIMEOUT,
    CONTENT_TYPE,
} from "@/constants";
import { useUserStore } from "@/stores/modules/user";

/**
 * 扩展的请求配置接口
 * 支持自定义消息提示行为
 */
export interface RequestConfig<D = unknown> extends AxiosRequestConfig<D> {
    /** 是否显示错误提示，默认 true */
    showErrorMessage?: boolean;
    /** 是否显示成功提示，默认 false */
    showSuccessMessage?: boolean;
    /** 自定义成功提示信息 */
    successMessage?: string;
    /** 是否显示 Loading 加载状态，默认 false */
    showLoading?: boolean;
}

// -------------------- 消息提示函数 --------------------

/**
 * 显示错误消息
 * @param message 错误信息
 */
const showErrorMessage = (message: string): void => {
    ElMessage({
        showClose: true,
        message,
        type: "error",
        duration: 3000,
        grouping: true, // 合并相同消息
    });
};

/**
 * 显示成功消息
 * @param message 成功信息
 */
const showSuccessMessage = (message: string): void => {
    ElMessage({
        showClose: true,
        message,
        type: "success",
        duration: 2000,
        grouping: true,
    });
};

/**
 * 显示警告消息
 * @param message 警告信息
 */
const showWarningMessage = (message: string): void => {
    ElMessage({
        showClose: true,
        message,
        type: "warning",
        duration: 2500,
        grouping: true,
    });
};

// -------------------- Axios 实例创建 --------------------

/**
 * 创建 Axios 实例
 * 使用 JSONBig 处理 JavaScript 中 int64 精度丢失问题
 */
const createAxiosInstance = (): AxiosInstance => {
    return axios.create({
        // 基础 URL，根据环境变量配置
        baseURL: import.meta.env.VITE_API_BASE_URL || "/api",
        // 请求超时时间
        timeout: REQUEST_TIMEOUT,
        // 请求头配置
        headers: {
            "Content-Type": CONTENT_TYPE.JSON,
        },
        // 自定义响应数据转换，解决 int64 精度问题
        transformResponse: [
            (data: string): unknown => {
                if (!data) return data;
                try {
                    // 使用 JSONBig 解析响应数据，保持大整数精度
                    return JSONBig.parse(data);
                } catch (error) {
                    // 解析失败时返回原始数据
                    console.warn("[JSON Parse Warning]:", error);
                    return data;
                }
            },
        ],
    });
};

/**
 * Axios 实例
 */
const service: AxiosInstance = createAxiosInstance();

// -------------------- 请求拦截器 --------------------

/**
 * 请求拦截器
 * 在请求发送前统一处理：添加 Token、设置请求头等
 */
service.interceptors.request.use(
    (config: InternalAxiosRequestConfig): InternalAxiosRequestConfig => {
        // 获取 Token 并添加到请求头
        const accessToken = useUserStore().getAccessToken;
        if (accessToken) {
            // 使用 Authorization 标准头
            config.headers.Authorization = accessToken;
        }

        if (config.data && !(config.data instanceof FormData) && !config.headers["Content-Type"]) {
            // 默认设置为 JSON 请求体
            config.headers["Content-Type"] = CONTENT_TYPE.JSON;
            // 对于非 FormData 的 JSON 请求体，进行深拷贝以防止数据污染
            config.data = JSON.parse(JSON.stringify(config.data));
        }

        // 开发环境日志
        if (import.meta.env.DEV) {
            console.log(`[Request] ${config.method?.toUpperCase()} ${config.url}`, config.data || config.params);
        }

        return config;
    },
    (error: AxiosError): Promise<never> => {
        console.error("[Request Error]:", error);
        showErrorMessage("请求配置错误");
        return Promise.reject(error);
    }
);

// -------------------- 响应拦截器 --------------------

/**
 * 响应拦截器
 * 统一处理响应数据和错误
 */
service.interceptors.response.use(
    (response: AxiosResponse<ApiResponse>): AxiosResponse<ApiResponse> => {
        const config = response.config as RequestConfig;
        const { data } = response;

        // 开发环境日志
        if (import.meta.env.DEV) {
            console.log(`[Response] ${config.url}`, data);
        }

        // 处理业务层面的成功响应
        if (data.success && data.code === BusinessErrorCode.SUCCESS) {
            // 如果配置了显示成功提示
            if (config.showSuccessMessage) {
                const message = config.successMessage || data.message || "操作成功";
                showSuccessMessage(message);
            }
            return response;
        }

        // 处理业务层面的失败响应
        const errorMessage = data.message || "操作失败";
        const errorCode = data.code;

        // 根据错误码特殊处理
        // 未登录或 Token 失效
        if (isAuthError(errorCode)) {
            if (config.showErrorMessage !== false) {
                showErrorMessage(errorMessage);
            }
            // 清除登录状态并跳转到登录页
            useUserStore().handleLogout();
        } else if (errorCode === BusinessErrorCode.FORBIDDEN) {
            // 无权限
            if (config.showErrorMessage !== false) {
                showErrorMessage(errorMessage);
            }
        } else if (errorCode === BusinessErrorCode.RATE_LIMIT) {
            // 请求频率过高
            if (config.showErrorMessage !== false) {
                showWarningMessage(errorMessage);
            }
        } else {
            // 其他业务错误
            if (config.showErrorMessage !== false) {
                showErrorMessage(errorMessage);
            }
        }

        // 返回响应，让业务层自行处理
        return response;
    },
    (error: AxiosError<ApiResponse>): Promise<never> => {
        // 获取请求配置
        const config = error.config as RequestConfig | undefined;
        const shouldShowError = config?.showErrorMessage !== false;

        // 开发环境日志
        if (import.meta.env.DEV) {
            console.error("[Response Error]:", error);
        }

        // 处理 HTTP 错误
        if (error.response) {
            const { status, data } = error.response;
            const errorMessage = data?.message || getHttpErrorMessage(status);

            if (shouldShowError) {
                showErrorMessage(errorMessage);
            }

            // 特殊处理未授权错误
            if (status === HttpStatusCode.UNAUTHORIZED) {
                useUserStore().handleLogout();
            }
        } else if (error.request) {
            // 请求已发送但未收到响应（网络错误）
            if (shouldShowError) {
                if (error.code === "ECONNABORTED") {
                    showErrorMessage("请求超时，请稍后重试");
                } else {
                    showErrorMessage("网络异常，请检查网络连接");
                }
            }
        } else {
            // 请求配置出错
            if (shouldShowError) {
                showErrorMessage("请求配置错误");
            }
        }

        return Promise.reject(error);
    }
);

// -------------------- 辅助函数 --------------------

/**
 * 根据 HTTP 状态码获取错误消息
 * @param status HTTP 状态码
 * @returns 错误消息
 */
const getHttpErrorMessage = (status: number): string => {
    return HTTP_ERROR_MESSAGES[status] || `请求失败 (${status})`;
};

// -------------------- 封装的请求方法 --------------------

/**
 * 封装 GET 请求
 * @param url 请求地址
 * @param params 查询参数
 * @param config 请求配置
 * @returns Promise
 */
export const get = <T = unknown>(
    url: string,
    params?: Record<string, unknown>,
    config?: RequestConfig
): Promise<AxiosResponse<ApiResponse<T>>> => {
    return service.get<ApiResponse<T>>(url, { params, ...config });
};

/**
 * 封装 POST 请求
 * @param url 请求地址
 * @param data 请求体数据
 * @param config 请求配置
 * @returns Promise
 */
export const post = <T = unknown>(
    url: string,
    data?: unknown,
    config?: RequestConfig
): Promise<AxiosResponse<ApiResponse<T>>> => {
    return service.post<ApiResponse<T>>(url, data, config);
};

/**
 * 封装 PUT 请求
 * @param url 请求地址
 * @param data 请求体数据
 * @param config 请求配置
 * @returns Promise
 */
export const put = <T = unknown>(
    url: string,
    data?: unknown,
    config?: RequestConfig
): Promise<AxiosResponse<ApiResponse<T>>> => {
    return service.put<ApiResponse<T>>(url, data, config);
};

/**
 * 封装 DELETE 请求
 * @param url 请求地址
 * @param params 查询参数
 * @param config 请求配置
 * @returns Promise
 */
export const del = <T = unknown>(
    url: string,
    params?: Record<string, unknown>,
    config?: RequestConfig
): Promise<AxiosResponse<ApiResponse<T>>> => {
    return service.delete<ApiResponse<T>>(url, { params, ...config });
};

/**
 * 封装 PATCH 请求
 * @param url 请求地址
 * @param data 请求体数据
 * @param config 请求配置
 * @returns Promise
 */
export const patch = <T = unknown>(
    url: string,
    data?: unknown,
    config?: RequestConfig
): Promise<AxiosResponse<ApiResponse<T>>> => {
    return service.patch<ApiResponse<T>>(url, data, config);
};

// -------------------- 导出 --------------------

/**
 * 导出 Axios 实例，供需要直接使用的场景
 */
export default service;
