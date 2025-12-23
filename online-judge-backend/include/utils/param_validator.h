#ifndef PARAM_VALIDATOR_H
#define PARAM_VALIDATOR_H

#include <httplib.h>
#include <json/json.h>

#include <string>
#include <vector>

namespace validator {

class ParamValidator {
public:
    /* -------------------- JSON Body 校验 -------------------- */
    /**
     * 检查 JSON 对象中是否包含指定的必填字段，且字段值不为空
     * @param root: JSON 对象
     * @param field: 需要检查的字段名称
     * @param errMsg: 错误信息输出参数（可选）
     * @return: 如果字段存在且不为空，返回 true；否则返回 false
     */
    static bool CheckRequired(const Json::Value &root, const std::string &field, std::string *errMsg = nullptr);

    /**
     * 检查 JSON 对象中是否包含指定的多个必填字段，且字段值不为空
     * @param root: JSON 对象
     * @param fields: 需要检查的字段名称列表
     * @param errMsg: 错误信息输出参数（可选）
     * @return: 如果所有字段都存在且不为空，返回 true；否则返回 false
     */
    static bool CheckRequiredList(const Json::Value &root, const std::vector<std::string> &fields,
                                  std::string *errMsg = nullptr);

    /**
     * 校验可选的数值类型的参数是否为有效数字且是正数
     * @param root       根 JSON 对象
     * @param field      数值字段名
     * @param errMsg    错误信息输出参数（可选）
     */
    static bool CheckPositiveNumber(const Json::Value &root, const std::string &field, std::string *errMsg = nullptr);

    /**
     * 校验可选 Object 参数的 Key 合法性
     * @param root       根 JSON 对象
     * @param field      Object 字段名（如 SearchInfo）
     * @param allowKeys  允许的 Key 列表
     */
    static bool CheckOptionalObjectKeys(const Json::Value &root, const std::string &field,
                                        const std::vector<std::string> &allowKeys, std::string *errMsg = nullptr);

    /* -------------------- GET Query 校验 -------------------- */
    /**
     * 检查 HTTP 请求的查询参数中是否包含指定的必填参数
     * @param req: HTTP 请求对象
     * @param param: 需要检查的参数名称
     * @param errMsg: 错误信息输出参数（可选）
     * @return: 如果参数存在，返回 true；否则返回 false
     */
    static bool CheckRequired(const httplib::Request &req, const std::string &param, std::string *errMsg = nullptr);

    /**
     * 检查 HTTP 请求的查询参数中是否包含指定的多个必填参数
     * @param req: HTTP 请求对象
     * @param params: 需要检查的参数名称列表
     * @param errMsg: 错误信息输出参数（可选）
     * @return: 如果所有参数都存在，返回 true；否则返回 false
     */
    static bool CheckRequiredList(const httplib::Request &req, const std::vector<std::string> &params,
                                  std::string *errMsg = nullptr);
};

}  // namespace validator

#endif  // PARAM_VALIDATOR_H
