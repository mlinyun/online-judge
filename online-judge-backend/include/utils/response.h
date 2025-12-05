/**
 * @file response.h
 * @brief 统一响应结构工具类
 * @description 提供统一的 API 响应构建方法，确保所有接口返回格式一致
 *
 * 响应结构：
 * {
 *     "success": true | false,
 *     "code": 0,
 *     "message": "操作说明消息",
 *     "data": { ... }  // 可选
 * }
 */
#ifndef RESPONSE_H
#define RESPONSE_H

#include <json/json.h>

#include <string>

#include "constants/error_code.h"

namespace response {

// -------------------- 核心响应构建函数 --------------------

/**
 * 构建响应 JSON
 * @param success 是否成功
 * @param code 状态码
 * @param message 消息说明
 * @param data 响应数据（可选）
 * @return Json::Value 响应 JSON
 */
inline Json::Value Build(bool success, int code, const std::string &message,
                         const Json::Value &data = Json::nullValue) {
    Json::Value json;
    json["success"] = success;
    json["code"] = code;
    json["message"] = message;
    if (!data.isNull()) {
        json["data"] = data;
    }
    return json;
}

// -------------------- 成功响应 --------------------

/**
 * 构建成功响应（无数据）
 * @param message 成功说明消息
 * @return Json::Value 响应 JSON
 */
inline Json::Value Success(const std::string &message = "操作成功") {
    return Build(true, error_code::SUCCESS, message);
}

/**
 * 构建成功响应（带数据）
 * @param message 成功说明消息
 * @param data 响应数据
 * @return Json::Value 响应 JSON
 */
inline Json::Value Success(const std::string &message, const Json::Value &data) {
    return Build(true, error_code::SUCCESS, message, data);
}

/**
 * 构建成功响应（仅数据，使用默认消息）
 * @param data 响应数据
 * @return Json::Value 响应 JSON
 */
inline Json::Value SuccessWithData(const Json::Value &data) {
    return Build(true, error_code::SUCCESS, "操作成功", data);
}

// -------------------- 失败响应 --------------------

/**
 * 构建失败响应
 * @param code 错误码
 * @param message 失败原因说明
 * @return Json::Value 响应 JSON
 */
inline Json::Value Fail(int code, const std::string &message) {
    return Build(false, code, message);
}

/**
 * 构建失败响应（使用默认错误码）
 * @param message 失败原因说明
 * @return Json::Value 响应 JSON
 */
inline Json::Value Fail(const std::string &message = "操作失败") {
    return Build(false, error_code::UNKNOWN_ERROR, message);
}

/**
 * 构建失败响应（带数据）
 * @param code 错误码
 * @param message 失败原因说明
 * @param data 附加数据
 * @return Json::Value 响应 JSON
 */
inline Json::Value Fail(int code, const std::string &message, const Json::Value &data) {
    Json::Value json = Build(false, code, message);
    json["data"] = data;
    return json;
}

// -------------------- 常用预定义响应 --------------------

/**
 * 参数错误响应
 * @param message 详细说明（可选）
 */
inline Json::Value BadRequest(const std::string &message = "请求参数有误") {
    return Fail(error_code::BAD_REQUEST, message);
}

/**
 * 未授权响应
 * @param message 详细说明（可选）
 */
inline Json::Value Unauthorized(const std::string &message = "未登录或登录已过期") {
    return Fail(error_code::UNAUTHORIZED, message);
}

/**
 * 无权限响应
 * @param message 详细说明（可选）
 */
inline Json::Value Forbidden(const std::string &message = "无操作权限") {
    return Fail(error_code::FORBIDDEN, message);
}

/**
 * 资源不存在响应
 * @param message 详细说明（可选）
 */
inline Json::Value NotFound(const std::string &message = "资源不存在") {
    return Fail(error_code::NOT_FOUND, message);
}

/**
 * 服务器内部错误响应
 * @param message 详细说明（可选）
 */
inline Json::Value InternalError(const std::string &message = "服务器内部错误") {
    return Fail(error_code::INTERNAL_ERROR, message);
}

/**
 * 数据库错误响应
 * @param message 详细说明（可选）
 */
inline Json::Value DatabaseError(const std::string &message = "数据库操作失败") {
    return Fail(error_code::DATABASE_ERROR, message);
}

// -------------------- 分页响应 --------------------

/**
 * 构建分页列表成功响应
 * @param message 成功说明消息
 * @param dataList 数据列表
 * @param total 数据总数
 * @return Json::Value 响应 JSON
 */
inline Json::Value SuccessList(const std::string &message, const Json::Value &dataList, int total) {
    Json::Value data;
    data["List"] = dataList;
    data["Total"] = total;
    return Build(true, error_code::SUCCESS, message, data);
}

/**
 * 构建分页列表成功响应（使用默认消息）
 * @param dataList 数据列表
 * @param total 数据总数
 * @return Json::Value 响应 JSON
 */
inline Json::Value SuccessList(const Json::Value &dataList, int total) {
    return SuccessList("查询成功", dataList, total);
}

// -------------------- 用户模块专用响应 --------------------

/**
 * 账户已存在响应
 */
inline Json::Value UserAccountExists(const std::string &message = "账户已存在，请重新填写！") {
    return Fail(error_code::USER_ACCOUNT_EXISTS, message);
}

/**
 * 昵称已存在响应
 */
inline Json::Value UserNicknameExists(const std::string &message = "昵称已存在，请重新填写！") {
    return Fail(error_code::USER_NICKNAME_EXISTS, message);
}

/**
 * 登录失败响应
 */
inline Json::Value UserLoginFailed(const std::string &message = "账户或密码错误！") {
    return Fail(error_code::USER_LOGIN_FAILED, message);
}

/**
 * 用户不存在响应
 */
inline Json::Value UserNotFound(const std::string &message = "用户不存在！") {
    return Fail(error_code::USER_NOT_FOUND, message);
}

/**
 * Token 无效响应
 */
inline Json::Value UserTokenInvalid(const std::string &message = "登录已过期，请重新登录！") {
    return Fail(error_code::USER_TOKEN_INVALID, message);
}

// -------------------- 题目模块专用响应 --------------------

/**
 * 题目不存在响应
 */
inline Json::Value ProblemNotFound(const std::string &message = "题目不存在！") {
    return Fail(error_code::PROBLEM_NOT_FOUND, message);
}

/**
 * 题目标题已存在响应
 */
inline Json::Value ProblemTitleExists(const std::string &message = "题目标题已存在！") {
    return Fail(error_code::PROBLEM_TITLE_EXISTS, message);
}

/**
 * 题目数据格式错误响应
 */
inline Json::Value ProblemDataInvalid(const std::string &message = "题目数据格式错误！") {
    return Fail(error_code::PROBLEM_DATA_INVALID, message);
}

// -------------------- 题解模块专用响应 --------------------
/**
 * 题解不存在响应
 */
inline Json::Value SolutionNotFound(const std::string &message = "题解不存在！") {
    return Fail(error_code::SOLUTION_NOT_FOUND, message);
}

// -------------------- 评论模块专用响应 --------------------

/**
 * 评论不存在响应
 */
inline Json::Value CommentNotFound(const std::string &message = "评论不存在！") {
    return Fail(error_code::COMMENT_NOT_FOUND, message);
}

/**
 * 评论内容为空响应
 */
inline Json::Value CommentContentEmpty(const std::string &message = "评论内容不能为空！") {
    return Fail(error_code::COMMENT_CONTENT_EMPTY, message);
}

/**
 * 评论类型无效响应
 */
inline Json::Value CommentTypeInvalid(const std::string &message = "评论类型无效！") {
    return Fail(error_code::COMMENT_TYPE_INVALID, message);
}

/**
 * 评论插入失败响应
 */
inline Json::Value CommentInsertFailed(const std::string &message = "评论插入失败！") {
    return Fail(error_code::COMMENT_INSERT_FAILED, message);
}

// -------------------- 测评记录模块专用响应 --------------------

/**
 * 测评记录不存在响应
 */
inline Json::Value StatusRecordNotFound(const std::string &message = "测评记录不存在！") {
    return Fail(error_code::STATUS_RECORD_NOT_FOUND, message);
}

// -------------------- 判题模块专用响应 --------------------

/**
 * 代码为空响应
 */
inline Json::Value JudgeCodeEmpty(const std::string &message = "提交代码不能为空！") {
    return Fail(error_code::JUDGE_CODE_EMPTY, message);
}

/**
 * 不支持的语言响应
 */
inline Json::Value JudgeLanguageUnsupported(const std::string &message = "不支持该编程语言！") {
    return Fail(error_code::JUDGE_LANGUAGE_UNSUPPORTED, message);
}

/**
 * 判题服务不可用响应
 */
inline Json::Value JudgeServiceUnavailable(const std::string &message = "判题服务暂时不可用！") {
    return Fail(error_code::JUDGE_SERVICE_UNAVAILABLE, message);
}

}  // namespace response

#endif  // RESPONSE_H
