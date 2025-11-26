#include "services/user_service.h"

#include "constants/user.h"     // 用户常量
#include "db/mongo_database.h"  // MongoDB 数据库操作类
#include "db/redis_database.h"  // Redis 数据库操作类
#include "utils/snowflake.hpp"  // 雪花算法

// 雪花算法
using snowflake_t = snowflake<1534832906275L, std::mutex>;
snowflake_t uuid_token;

// 局部静态特性的方式实现单实例模式
UserService *UserService::GetInstance() {
    static UserService user_service;
    return &user_service;
}

// 用户注册（Token鉴权实现）
Json::Value UserService::RegisterUser(Json::Value &registerjson) {
    Json::Value json = MoDB::GetInstance()->RegisterUser(registerjson);
    // 将其权限加入用户权限表中
    if (json["Result"].asString() == "Success") {
        int64_t id = json["_id"].asInt64();
        // 注册默认普通用户权限
        UserAuthorityMap[id] = constants::user::USER_AUTHORITY_ORDINARY;
    }
    return json;
}

// 用户登录（Token鉴权实现）
Json::Value UserService::LoginUser(Json::Value &loginjson) {
    Json::Value json = MoDB::GetInstance()->LoginUser(loginjson);
    if (json["Result"] == "Success") {
        uuid_token.init(1, 1);
        // 获取Token
        int64_t token = uuid_token.nextid();

        // Redis 存入 Token
        ReDB::GetInstance()->SetToken(to_string(token), json["Info"]["_id"].asString());

        json["Info"]["Token"] = to_string(token);
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

// 删除用户（Token鉴权实现）
Json::Value UserService::DeleteUser(Json::Value &deletejson) {
    Json::Value json = MoDB::GetInstance()->SelectUserInfo(deletejson);
    if (json["Result"] == "Success") {
        int64_t id = stoll(deletejson["UserId"].asString());
        UserAuthorityMap.erase(id);
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

// 登录用户通过 Token
Json::Value UserService::LoginUserByToken(Json::Value &loginjson) {
    string token = loginjson["Token"].asString();

    // 根据 Token 查询 UserId
    string userid = ReDB::GetInstance()->GetUserIdByToken(token);

    loginjson["UserId"] = userid;

    return MoDB::GetInstance()->LoginUserByToken(loginjson);
}

// 初始化用户权限
bool UserService::InitUserAuthority() {
    Json::Value json = MoDB::GetInstance()->SelectUserAuthority();
    if (json["Result"] == "Fail") {
        return false;
    }
    for (auto info : json["ArrayInfo"]) {
        UserAuthorityMap[info["_id"].asInt64()] = info["Authority"].asInt();
    }
    return true;
}

// 将 json 的 Token 转化为 VerifyId
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
        if (UserAuthorityMap[id] == 0)
            return constants::user::USER_AUTHORITY_GUEST;
        return UserAuthorityMap[id];
    } catch (const std::exception &e) {
        // 如果出现异常则返回游客权限
        return constants::user::USER_AUTHORITY_GUEST;
    }
}

// 是否是普通用户或以上
bool UserService::IsOrdinaryUser(Json::Value &json) {
    TokenToVerifyId(json);
    if (GetUserAuthority(json) >= constants::user::USER_AUTHORITY_ORDINARY) {
        return true;
    } else {
        return false;
    }
}

// 是否是作者本人或以上
bool UserService::IsAuthor(Json::Value &json) {
    TokenToVerifyId(json);
    int authority = GetUserAuthority(json);
    if (authority < constants::user::USER_AUTHORITY_ORDINARY) {
        return false;
    }
    if (authority >= constants::user::USER_AUTHORITY_ADMINISTRATOR) {
        return true;
    }
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

// 是否是管理员或以上
bool UserService::IsAdministrator(Json::Value &json) {
    TokenToVerifyId(json);
    if (GetUserAuthority(json) >= constants::user::USER_AUTHORITY_ADMINISTRATOR) {
        return true;
    } else {
        return false;
    }
}

UserService::UserService() {
    // 构造函数实现
}

UserService::~UserService() {
    // 析构函数实现
}