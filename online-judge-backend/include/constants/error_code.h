/**
 * @file error_code.h
 * @brief 统一错误码定义
 * @description 定义系统所有的错误码，便于前后端统一处理
 *
 * 错误码规则：
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
 */
#ifndef ERROR_CODE_H
#define ERROR_CODE_H

namespace error_code {

// ==================== 成功 ====================
/** 操作成功 */
constexpr int SUCCESS = 0;

// ==================== 通用错误 (1xxx) ====================
/** 未知错误 */
constexpr int UNKNOWN_ERROR = 1000;
/** 请求参数错误 */
constexpr int BAD_REQUEST = 1001;
/** 未授权（未登录） */
constexpr int UNAUTHORIZED = 1002;
/** 无操作权限 */
constexpr int FORBIDDEN = 1003;
/** 资源不存在 */
constexpr int NOT_FOUND = 1004;
/** 服务器内部错误 */
constexpr int INTERNAL_ERROR = 1005;
/** 数据库错误 */
constexpr int DATABASE_ERROR = 1006;
/** 请求频率过高 */
constexpr int RATE_LIMIT = 1007;

// ==================== 用户模块错误 (2xxx) ====================
/** 账户已存在 */
constexpr int USER_ACCOUNT_EXISTS = 2001;
/** 昵称已存在 */
constexpr int USER_NICKNAME_EXISTS = 2002;
/** 账户或密码错误 */
constexpr int USER_LOGIN_FAILED = 2003;
/** 用户不存在 */
constexpr int USER_NOT_FOUND = 2004;
/** 密码格式错误 */
constexpr int USER_PASSWORD_INVALID = 2005;
/** 旧密码错误 */
constexpr int USER_OLD_PASSWORD_WRONG = 2006;
/** Token 无效或过期 */
constexpr int USER_TOKEN_INVALID = 2007;

// ==================== 题目模块错误 (3xxx) ====================
/** 题目不存在 */
constexpr int PROBLEM_NOT_FOUND = 3001;
/** 题目标题已存在 */
constexpr int PROBLEM_TITLE_EXISTS = 3002;
/** 题目数据格式错误 */
constexpr int PROBLEM_DATA_INVALID = 3003;

// ==================== 公告模块错误 (4xxx) ====================
/** 公告不存在 */
constexpr int ANNOUNCEMENT_NOT_FOUND = 4001;

// ==================== 讨论模块错误 (5xxx) ====================
/** 讨论不存在 */
constexpr int DISCUSS_NOT_FOUND = 5001;

// ==================== 题解模块错误 (6xxx) ====================
/** 题解不存在 */
constexpr int SOLUTION_NOT_FOUND = 6001;

// ==================== 评论模块错误 (7xxx) ====================
/** 评论不存在 */
constexpr int COMMENT_NOT_FOUND = 7001;
/** 评论内容为空 */
constexpr int COMMENT_CONTENT_EMPTY = 7002;
/** 评论类型无效 */
constexpr int COMMENT_TYPE_INVALID = 7003;
/** 评论插入失败 */
constexpr int COMMENT_INSERT_FAILED = 7004;

// ==================== 测评记录模块错误 (8xxx) ====================
/** 测评记录不存在 */
constexpr int STATUS_RECORD_NOT_FOUND = 8001;

// ==================== 判题模块错误 (9xxx) ====================
/** 代码为空 */
constexpr int JUDGE_CODE_EMPTY = 9001;
/** 不支持的语言 */
constexpr int JUDGE_LANGUAGE_UNSUPPORTED = 9002;
/** 判题服务不可用 */
constexpr int JUDGE_SERVICE_UNAVAILABLE = 9003;

}  // namespace error_code

#endif  // ERROR_CODE_H
