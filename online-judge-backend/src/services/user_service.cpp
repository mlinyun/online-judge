#include "services/user_service.h"

#include "constants/user.h"        // 用户常量
#include "db/mongo_database.h"     // MongoDB 数据库操作类
#include "db/redis_database.h"     // Redis 数据库操作类
#include "utils/id_generator.hpp"  // 唯一 ID 生成器
#include "utils/response.h"        // 统一响应工具

// 局部静态特性的方式实现单实例模式
UserService *UserService::GetInstance() {
    static UserService user_service;
    return &user_service;
}

// 用户注册（Token 鉴权实现）
Json::Value UserService::UserRegister(Json::Value &registerjson) {
    Json::Value json = MoDB::GetInstance()->UserRegister(registerjson);
    // 将其权限加入用户权限表中
    if (json["success"].asBool()) {
        int64_t id = stoll(json["data"]["UserId"].asString());
        // 注册默认普通用户权限
        UserAuthorityMap[id] = constants::user::USER_AUTHORITY_ORDINARY;
    }
    return json;
}

// 用户登录（通过用户账号 + 密码进行登录，登录成功后将 Token 存入 Redis）
Json::Value UserService::UserLogin(Json::Value &loginjson) {
    Json::Value json = MoDB::GetInstance()->UserLogin(loginjson);
    if (json["success"].asBool() && json["data"].isObject()) {
        // 使用雪花算法生成唯一 Token
        int64_t token = IDGenerator::Instance().NextId();
        std::string user_id;
        if (json["data"]["_id"].isString()) {
            user_id = json["data"]["_id"].asString();
        } else if (json["data"]["_id"].isIntegral()) {
            user_id = std::to_string(json["data"]["_id"].asInt64());
        } else {
            user_id = json["data"]["_id"].asString();
        }
        // 将 Token 存入 Redis：键为 Token，值为 UserId
        ReDB::GetInstance()->SetToken(to_string(token), user_id);
        // 返回 Token
        json["data"]["Token"] = to_string(token);
    }
    return json;
}

// 查询用户信息
Json::Value UserService::SelectUserInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserInfo(queryjson);
}

// 查询用户信息（在设置页面修改用户时使用）
Json::Value UserService::SelectUserUpdateInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 更新用户信息
Json::Value UserService::UpdateUserInfo(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateUserInfo(updatejson);
}

// 删除用户（Token 鉴权实现）
Json::Value UserService::DeleteUser(Json::Value &deletejson) {
    Json::Value json = MoDB::GetInstance()->DeleteUser(deletejson);
    if (json["success"].asBool()) {
        // 用户删除成功后，从用户权限表中删除该用户的权限记录
        int64_t id = stoll(deletejson["UserId"].asString());
        UserAuthorityMap.erase(id);
        // 同时删除该用户的所有 Token 记录
        ReDB::GetInstance()->DeleteTokensByUserId(to_string(id));
    }
    return json;
}

// 用户排名查询
Json::Value UserService::SelectUserRank(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserRank(queryjson);
}

// 分页查询用户列表（管理员权限）
Json::Value UserService::SelectUserSetInfo(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectUserSetInfo(queryjson);
}

// 更新用户题目信息（用于用户提交代码后更新题目完成情况）
bool UserService::UpdateUserProblemInfo(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateUserProblemInfo(updatejson);
}

// 通过 Token 获取用户 ID
std::string UserService::GetUserIdByToken(const std::string &token) {
    return ReDB::GetInstance()->GetUserIdByToken(token);
}

// 通过 UserId 获取用户名 NickName
std::string UserService::GetNickNameByUserId(const std::string &userid) {
    return MoDB::GetInstance()->GetNickNameByUserId(userid);
}

// 登录用户通过 Token
Json::Value UserService::LoginUserByToken(Json::Value &loginjson) {
    string token = loginjson["Token"].asString();

    // 根据 Token 查询 UserId
    string userid = GetUserIdByToken(token);

    loginjson["UserId"] = userid;

    return MoDB::GetInstance()->LoginUserByToken(loginjson);
}

// 初始化用户权限：从数据库中加载所有用户的权限到 UserAuthorityMap 中
bool UserService::InitUserAuthority() {
    Json::Value json = MoDB::GetInstance()->SelectUserAuthority();
    if (!json["success"].asBool() || !json["data"].isObject() || !json["data"]["list"].isArray()) {
        return false;
    }
    for (auto info : json["data"]["list"]) {
        UserAuthorityMap[info["_id"].asInt64()] = info["Authority"].asInt();
    }
    return true;
}

// 检查用户是否已登录
Json::Value UserService::CheckLogin(Json::Value &json) {
    // 获取 Token
    std::string token = json["Token"].asString();
    return CheckLoginByToken(token);
}

// 检查用户是否已登录（通过 Token）
Json::Value UserService::CheckLoginByToken(const std::string &token) {
    // Token 为空，未登录
    if (token.empty()) {
        return response::Unauthorized("未登录，请先登录！");
    }
    // 从 Redis 中获取用户 ID
    std::string userid = GetUserIdByToken(token);
    // 用户 ID 为空或为 "0"，Token 无效或已过期
    if (userid.empty() || userid == "0") {
        return response::UserTokenInvalid("登录已过期，请重新登录！");
    }
    // 登录有效，返回空 Json 表示通过
    return Json::Value();
}

// 将 Json 的 Token 转化为 VerifyId
void TokenToVerifyId(Json::Value &json) {
    if (!json["Token"].isNull()) {
        json["VerifyId"] = ReDB::GetInstance()->GetUserIdByToken(json["Token"].asString());
    }
}

// 获取用户权限
int UserService::GetUserAuthority(Json::Value &json) {
    // 如果未发现 VerifyId，说明是游客
    if (json["VerifyId"].isNull()) {
        return constants::user::USER_AUTHORITY_GUEST;
    }
    try {
        int64_t id = stoll(json["VerifyId"].asString());
        // 如果未查询到该用户 ID 或者用户 ID 为 0 则是游客
        if (UserAuthorityMap[id] == 0) {
            return constants::user::USER_AUTHORITY_GUEST;
        }
        // 返回从哈希表中查询到的用户权限：键为用户 ID，值为用户权限
        return UserAuthorityMap[id];
    } catch (const std::exception &e) {
        // 如果出现异常则返回游客权限
        return constants::user::USER_AUTHORITY_GUEST;
    }
}

// 是否是普通用户
bool UserService::IsOrdinaryUser(Json::Value &json) {
    TokenToVerifyId(json);
    if (GetUserAuthority(json) == constants::user::USER_AUTHORITY_ORDINARY) {
        // 如果是普通用户，返回 true
        return true;
    } else {
        // 其他情况返回 false
        return false;
    }
}

// 是否是作者本人
bool UserService::IsAuthor(Json::Value &json) {
    TokenToVerifyId(json);
    int authority = GetUserAuthority(json);
    if (authority < constants::user::USER_AUTHORITY_ORDINARY) {
        // 游客权限，直接返回 false
        return false;
    }
    // 其他情况需要判断是否是作者本人，即判断 VerifyId 和 UserId 是否相等
    try {
        int64_t verifyid = stoll(json["VerifyId"].asString());
        int64_t userid = stoll(json["UserId"].asString());

        if (verifyid == userid) {
            return true;
        } else {
            return false;
        }
    } catch (const std::exception &e) {
        return false;
    }
}

// 是否是管理员
bool UserService::IsAdministrator(Json::Value &json) {
    TokenToVerifyId(json);
    if (GetUserAuthority(json) == constants::user::USER_AUTHORITY_ADMINISTRATOR) {
        // 管理员权限，直接返回 true
        return true;
    } else {
        // 非管理员权限，返回 false
        return false;
    }
}

// 是否是普通用户或以上（即可普通用户 + 管理员）
bool UserService::IsOrdinaryUserOrAbove(Json::Value &json) {
    TokenToVerifyId(json);
    if (GetUserAuthority(json) >= constants::user::USER_AUTHORITY_ORDINARY) {
        // 如果是普通用户或者是管理员，返回 true
        return true;
    } else {
        // 其他情况返回 false
        return false;
    }
}

// 是否是作者本人或以上（即需要是普通用户且是作者本人，或者是管理员）
bool UserService::IsAuthorOrAbove(Json::Value &json) {
    TokenToVerifyId(json);
    int authority = GetUserAuthority(json);
    if (authority < constants::user::USER_AUTHORITY_ORDINARY) {
        // 游客权限，直接返回 false
        return false;
    }
    if (authority == constants::user::USER_AUTHORITY_ADMINISTRATOR) {
        // 管理员权限，直接返回 true
        return true;
    }
    // 如果是普通用户权限，需要判断是否是作者本人，即判断 VerifyId 和 UserId 是否相等
    try {
        int64_t verifyid = stoll(json["VerifyId"].asString());
        int64_t userid = stoll(json["UserId"].asString());

        if (verifyid == userid) {
            return true;
        } else {
            return false;
        }
    } catch (const std::exception &e) {
        return false;
    }
}

UserService::UserService() {
    // 构造函数实现
}

UserService::~UserService() {
    // 析构函数实现
}