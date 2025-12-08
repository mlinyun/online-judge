/**
 * HTTP 相关常量定义
 *
 * 提供 HTTP 状态码、请求配置等常量
 *
 * ## 主要功能
 *
 * - HTTP 状态码枚举
 * - 请求超时配置
 * - 内容类型定义
 *
 * @module constants/http
 */

/**
 * HTTP 状态码枚举
 *
 * 定义常用的 HTTP 响应状态码
 */
export enum HttpStatusCode {
    /** 成功 */
    OK = 200,
    /** 已创建 */
    CREATED = 201,
    /** 无内容 */
    NO_CONTENT = 204,
    /** 请求参数错误 */
    BAD_REQUEST = 400,
    /** 未授权（未登录） */
    UNAUTHORIZED = 401,
    /** 无权限 */
    FORBIDDEN = 403,
    /** 资源不存在 */
    NOT_FOUND = 404,
    /** 请求超时 */
    REQUEST_TIMEOUT = 408,
    /** 请求冲突 */
    CONFLICT = 409,
    /** 请求频率过高 */
    TOO_MANY_REQUESTS = 429,
    /** 服务器内部错误 */
    INTERNAL_SERVER_ERROR = 500,
    /** 网关错误 */
    BAD_GATEWAY = 502,
    /** 服务不可用 */
    SERVICE_UNAVAILABLE = 503,
    /** 网关超时 */
    GATEWAY_TIMEOUT = 504,
}

/**
 * HTTP 状态码错误消息映射
 */
export const HTTP_ERROR_MESSAGES: Record<number, string> = {
    [HttpStatusCode.BAD_REQUEST]: "请求参数有误",
    [HttpStatusCode.UNAUTHORIZED]: "未登录或登录已过期",
    [HttpStatusCode.FORBIDDEN]: "无权限访问该资源",
    [HttpStatusCode.NOT_FOUND]: "请求的资源不存在",
    [HttpStatusCode.REQUEST_TIMEOUT]: "请求超时",
    [HttpStatusCode.CONFLICT]: "请求冲突",
    [HttpStatusCode.TOO_MANY_REQUESTS]: "请求过于频繁，请稍后重试",
    [HttpStatusCode.INTERNAL_SERVER_ERROR]: "服务器出错，请稍后重试",
    [HttpStatusCode.BAD_GATEWAY]: "网关错误",
    [HttpStatusCode.SERVICE_UNAVAILABLE]: "服务暂时不可用",
    [HttpStatusCode.GATEWAY_TIMEOUT]: "网关超时",
};

/**
 * 请求超时时间（毫秒）
 */
export const REQUEST_TIMEOUT = 30000;

/**
 * 内容类型常量
 */
export const CONTENT_TYPE = {
    /** JSON 格式 */
    JSON: "application/json;charset=UTF-8",
    /** 表单格式 */
    FORM: "application/x-www-form-urlencoded;charset=UTF-8",
    /** 文件上传 */
    MULTIPART: "multipart/form-data",
    /** 纯文本 */
    TEXT: "text/plain;charset=UTF-8",
} as const;
