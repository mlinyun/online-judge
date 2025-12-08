/**
 * 业务错误码定义
 *
 * 与后端 error_code.h 保持完全一致
 *
 * ## 错误码规则
 *
 * - 0: 成功
 * - 1xxx: 通用错误
 * - 2xxx: 用户模块错误
 * - 3xxx: 题目模块错误
 * - 4xxx: 公告模块错误
 * - 5xxx: 讨论模块错误
 * - 6xxx: 题解模块错误
 * - 7xxx: 评论模块错误
 * - 8xxx: 测评记录模块错误
 * - 9xxx: 判题模块错误
 *
 * @module constants/error-code
 */

/**
 * 业务错误码枚举
 *
 * 与后端 `include/constants/error_code.h` 保持一致
 */
export enum BusinessErrorCode {
    // ==================== 成功 ====================
    /** 操作成功 */
    SUCCESS = 0,

    // ==================== 通用错误 (1xxx) ====================
    /** 未知错误 */
    UNKNOWN_ERROR = 1000,
    /** 请求参数错误 */
    BAD_REQUEST = 1001,
    /** 未授权（未登录） */
    UNAUTHORIZED = 1002,
    /** 无操作权限 */
    FORBIDDEN = 1003,
    /** 资源不存在 */
    NOT_FOUND = 1004,
    /** 服务器内部错误 */
    INTERNAL_ERROR = 1005,
    /** 数据库错误 */
    DATABASE_ERROR = 1006,
    /** 请求频率过高 */
    RATE_LIMIT = 1007,

    // ==================== 用户模块错误 (2xxx) ====================
    /** 账户已存在 */
    USER_ACCOUNT_EXISTS = 2001,
    /** 昵称已存在 */
    USER_NICKNAME_EXISTS = 2002,
    /** 账户或密码错误 */
    USER_LOGIN_FAILED = 2003,
    /** 用户不存在 */
    USER_NOT_FOUND = 2004,
    /** 密码格式错误 */
    USER_PASSWORD_INVALID = 2005,
    /** 旧密码错误 */
    USER_OLD_PASSWORD_WRONG = 2006,
    /** Token 无效或过期 */
    USER_TOKEN_INVALID = 2007,

    // ==================== 题目模块错误 (3xxx) ====================
    /** 题目不存在 */
    PROBLEM_NOT_FOUND = 3001,
    /** 题目标题已存在 */
    PROBLEM_TITLE_EXISTS = 3002,
    /** 题目数据格式错误 */
    PROBLEM_DATA_INVALID = 3003,

    // ==================== 公告模块错误 (4xxx) ====================
    /** 公告不存在 */
    ANNOUNCEMENT_NOT_FOUND = 4001,

    // ==================== 讨论模块错误 (5xxx) ====================
    /** 讨论不存在 */
    DISCUSS_NOT_FOUND = 5001,

    // ==================== 题解模块错误 (6xxx) ====================
    /** 题解不存在 */
    SOLUTION_NOT_FOUND = 6001,

    // ==================== 评论模块错误 (7xxx) ====================
    /** 评论不存在 */
    COMMENT_NOT_FOUND = 7001,
    /** 评论内容为空 */
    COMMENT_CONTENT_EMPTY = 7002,
    /** 评论类型无效 */
    COMMENT_TYPE_INVALID = 7003,
    /** 评论插入失败 */
    COMMENT_INSERT_FAILED = 7004,

    // ==================== 测评记录模块错误 (8xxx) ====================
    /** 测评记录不存在 */
    STATUS_RECORD_NOT_FOUND = 8001,

    // ==================== 判题模块错误 (9xxx) ====================
    /** 代码为空 */
    JUDGE_CODE_EMPTY = 9001,
    /** 不支持的语言 */
    JUDGE_LANGUAGE_UNSUPPORTED = 9002,
    /** 判题服务不可用 */
    JUDGE_SERVICE_UNAVAILABLE = 9003,
}

/**
 * 错误码描述映射
 *
 * 提供错误码的中文描述，便于调试和日志记录
 */
export const ERROR_CODE_MESSAGES: Record<BusinessErrorCode, string> = {
    // 成功
    [BusinessErrorCode.SUCCESS]: "操作成功",

    // 通用错误
    [BusinessErrorCode.UNKNOWN_ERROR]: "未知错误",
    [BusinessErrorCode.BAD_REQUEST]: "请求参数错误",
    [BusinessErrorCode.UNAUTHORIZED]: "未授权（未登录）",
    [BusinessErrorCode.FORBIDDEN]: "无操作权限",
    [BusinessErrorCode.NOT_FOUND]: "资源不存在",
    [BusinessErrorCode.INTERNAL_ERROR]: "服务器内部错误",
    [BusinessErrorCode.DATABASE_ERROR]: "数据库错误",
    [BusinessErrorCode.RATE_LIMIT]: "请求频率过高",

    // 用户模块错误
    [BusinessErrorCode.USER_ACCOUNT_EXISTS]: "账户已存在",
    [BusinessErrorCode.USER_NICKNAME_EXISTS]: "昵称已存在",
    [BusinessErrorCode.USER_LOGIN_FAILED]: "账户或密码错误",
    [BusinessErrorCode.USER_NOT_FOUND]: "用户不存在",
    [BusinessErrorCode.USER_PASSWORD_INVALID]: "密码格式错误",
    [BusinessErrorCode.USER_OLD_PASSWORD_WRONG]: "旧密码错误",
    [BusinessErrorCode.USER_TOKEN_INVALID]: "Token 无效或过期",

    // 题目模块错误
    [BusinessErrorCode.PROBLEM_NOT_FOUND]: "题目不存在",
    [BusinessErrorCode.PROBLEM_TITLE_EXISTS]: "题目标题已存在",
    [BusinessErrorCode.PROBLEM_DATA_INVALID]: "题目数据格式错误",

    // 公告模块错误
    [BusinessErrorCode.ANNOUNCEMENT_NOT_FOUND]: "公告不存在",

    // 讨论模块错误
    [BusinessErrorCode.DISCUSS_NOT_FOUND]: "讨论不存在",

    // 题解模块错误
    [BusinessErrorCode.SOLUTION_NOT_FOUND]: "题解不存在",

    // 评论模块错误
    [BusinessErrorCode.COMMENT_NOT_FOUND]: "评论不存在",
    [BusinessErrorCode.COMMENT_CONTENT_EMPTY]: "评论内容为空",
    [BusinessErrorCode.COMMENT_TYPE_INVALID]: "评论类型无效",
    [BusinessErrorCode.COMMENT_INSERT_FAILED]: "评论插入失败",

    // 测评记录模块错误
    [BusinessErrorCode.STATUS_RECORD_NOT_FOUND]: "测评记录不存在",

    // 判题模块错误
    [BusinessErrorCode.JUDGE_CODE_EMPTY]: "代码为空",
    [BusinessErrorCode.JUDGE_LANGUAGE_UNSUPPORTED]: "不支持的语言",
    [BusinessErrorCode.JUDGE_SERVICE_UNAVAILABLE]: "判题服务不可用",
};

/**
 * 判断是否为成功状态
 * @param code 错误码
 * @returns 是否成功
 */
export const isSuccess = (code: number): boolean => {
    return code === BusinessErrorCode.SUCCESS;
};

/**
 * 判断是否为认证相关错误
 * @param code 错误码
 * @returns 是否为认证错误
 */
export const isAuthError = (code: number): boolean => {
    return code === BusinessErrorCode.UNAUTHORIZED || code === BusinessErrorCode.USER_TOKEN_INVALID;
};

/**
 * 判断是否为权限相关错误
 * @param code 错误码
 * @returns 是否为权限错误
 */
export const isPermissionError = (code: number): boolean => {
    return code === BusinessErrorCode.FORBIDDEN;
};

/**
 * 获取错误码描述
 * @param code 错误码
 * @returns 错误描述
 */
export const getErrorMessage = (code: number): string => {
    return ERROR_CODE_MESSAGES[code as BusinessErrorCode] || "未知错误";
};
