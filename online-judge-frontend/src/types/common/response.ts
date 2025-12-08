/**
 * API 响应类型定义模块
 *
 * @description 提供统一的 API 响应结构类型定义
 *
 * ## 主要功能
 *
 * - 基础响应结构定义
 * - 泛型支持（适配不同数据类型）
 * - 统一的响应格式约束
 *
 * ## 使用场景
 *
 * - API 请求响应类型约束
 * - 接口数据类型定义
 * - 响应数据解析
 *
 * @module types/common/response
 */

/** 基础 API 响应结构定义 */
export interface ApiResponse<T = unknown> {
    /** 请求是否成功 */
    success: boolean;
    /** 状态码（0 表示成功，其余表示错误码） */
    code: number;
    /** 说明消息 */
    message: string;
    /** 响应数据 */
    data?: T;
}
