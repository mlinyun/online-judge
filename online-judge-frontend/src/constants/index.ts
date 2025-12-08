/**
 * 常量统一导出模块
 *
 * 提供所有常量的统一导出入口
 *
 * ## 使用方式
 *
 * ```typescript
 * // 导入所有常量
 * import { HttpStatusCode, BusinessErrorCode, STORAGE_KEYS } from "@/constants";
 *
 * // 或按需导入
 * import { HttpStatusCode } from "@/constants/http";
 * import { BusinessErrorCode } from "@/constants/error-code";
 * ```
 *
 * @module constants
 */

// HTTP 相关
export { HttpStatusCode, HTTP_ERROR_MESSAGES, REQUEST_TIMEOUT, CONTENT_TYPE } from "./http";

// 错误码相关
export {
    BusinessErrorCode,
    ERROR_CODE_MESSAGES,
    isSuccess,
    isAuthError,
    isPermissionError,
    getErrorMessage,
} from "./error-code";

// 配置相关
export { STORAGE_KEYS, APP_CONFIG, API_CONFIG } from "./config";

// 用户相关
export { UserAuthority } from "./user";
