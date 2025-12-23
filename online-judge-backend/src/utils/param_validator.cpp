#include "utils/param_validator.h"

#include <algorithm>

namespace validator {

static bool IsBlankString(const std::string &str) {
    return std::all_of(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
}

/* -------------------- JSON Body 校验 -------------------- */

bool ParamValidator::CheckRequired(const Json::Value &root, const std::string &field, std::string *errMsg) {
    if (!root.isMember(field)) {
        if (errMsg) {
            *errMsg = "Missing required parameter: " + field;
        }
        return false;
    }

    const Json::Value &value = root[field];

    if (value.isNull()) {
        if (errMsg) {
            *errMsg = "Parameter is null: " + field;
        }
        return false;
    }

    if (value.isString()) {
        const std::string str = value.asString();
        if (str.empty() || IsBlankString(str)) {
            if (errMsg) {
                *errMsg = "Parameter is empty: " + field;
            }
            return false;
        }
    }

    return true;
}

bool ParamValidator::CheckRequiredList(const Json::Value &root, const std::vector<std::string> &fields,
                                       std::string *errMsg) {
    for (const auto &field : fields) {
        if (!CheckRequired(root, field, errMsg)) {
            return false;
        }
    }
    return true;
}

bool ParamValidator::CheckPositiveNumber(const Json::Value &root, const std::string &field, std::string *errMsg) {
    if (root.isMember(field)) {
        // 字段存在，继续校验
        const Json::Value &value = root[field];

        if (!value.isNumeric()) {
            if (errMsg) {
                *errMsg = "Parameter is not a valid number: " + field;
            }
            return false;
        }

        double number = value.asDouble();
        if (number <= 0) {
            if (errMsg) {
                *errMsg = "Parameter must be a positive number: " + field;
            }
            return false;
        }
    }
    // 字段不存在，直接放行（可选参数）
    return true;
}

bool ParamValidator::CheckOptionalObjectKeys(const Json::Value &root, const std::string &field,
                                             const std::vector<std::string> &allowKeys, std::string *errMsg) {
    // 未传可选字段，直接放行
    if (!root.isMember(field)) {
        return true;
    }

    const Json::Value &obj = root[field];

    // 可选字段必须是 Object
    if (!obj.isObject()) {
        if (errMsg) {
            *errMsg = field + " must be an object";
        }
        return false;
    }

    // 构建可选字段允许 Key 集合
    std::unordered_set<std::string> allowSet(allowKeys.begin(), allowKeys.end());

    // 校验可选字段是否存在非法 Key
    for (const auto &key : obj.getMemberNames()) {
        if (allowSet.find(key) == allowSet.end()) {
            if (errMsg) {
                *errMsg = "Invalid key in " + field + ": " + key;
            }
            return false;
        }
    }

    // 校验可选字段中的 Key 对应的值是否合法（非空）
    for (const auto &key : obj.getMemberNames()) {
        const Json::Value &value = obj[key];

        // null
        if (value.isNull()) {
            if (errMsg) {
                *errMsg = field + "." + key + " must not be null";
            }
            return false;
        }

        // string
        if (value.isString()) {
            std::string str = value.asString();
            if (str.empty() || std::all_of(str.begin(), str.end(), ::isspace)) {
                if (errMsg) {
                    *errMsg = field + "." + key + " must not be empty";
                }
                return false;
            }
        }

        // array
        if (value.isArray() && value.empty()) {
            if (errMsg) {
                *errMsg = field + "." + key + " must not be empty array";
            }
            return false;
        }

        // object（可选规则，推荐）
        if (value.isObject() && value.empty()) {
            if (errMsg) {
                *errMsg = field + "." + key + " must not be empty object";
            }
            return false;
        }
    }

    return true;
}

/* -------------------- GET Query 校验 -------------------- */

bool ParamValidator::CheckRequired(const httplib::Request &req, const std::string &param, std::string *errMsg) {
    if (!req.has_param(param)) {
        if (errMsg) {
            *errMsg = "Missing required query parameter: " + param;
        }
        return false;
    }

    const std::string value = req.get_param_value(param);
    if (value.empty() || IsBlankString(value)) {
        if (errMsg) {
            *errMsg = "Query parameter is empty: " + param;
        }
        return false;
    }

    return true;
}

bool ParamValidator::CheckRequiredList(const httplib::Request &req, const std::vector<std::string> &params,
                                       std::string *errMsg) {
    for (const auto &param : params) {
        if (!CheckRequired(req, param, errMsg)) {
            return false;
        }
    }
    return true;
}

}  // namespace validator
