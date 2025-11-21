#include "UserList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include "./utils/snowflake.hpp"
#include <iostream>

using namespace std;

// 雪花算法
using snowflake_t = snowflake<1534832906275L, std::mutex>;
snowflake_t uuid_token;

// 注册用户（Token鉴权实现）
Json::Value UserList::RegisterUser(Json::Value &registerjson)
{
    Json::Value json = MoDB::GetInstance()->RegisterUser(registerjson);
    // 将其权限加入用户权限表中
    if (json["Result"].asString() == "Success")
    {
        int64_t id = json["_id"].asInt64();
        UserAuthorityMap[id] = 3;
    }
    return json;
}

// 登录用户（Token鉴权实现）
Json::Value UserList::LoginUser(Json::Value &loginjson)
{
    Json::Value json = MoDB::GetInstance()->LoginUser(loginjson);

    if (json["Result"] == "Success")
    {
        uuid_token.init(1, 1);
        // 获取Token
        int64_t token = uuid_token.nextid();

        // Redis存入Token
        ReDB::GetInstance()->SetToken(to_string(token), json["Info"]["_id"].asString());

        json["Info"]["Token"] = to_string(token);
    }

    return json;
}

// 获取用户大部分信息
Json::Value UserList::SelectUserInfo(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectUserInfo(queryjson);
}

// 更新用户信息
Json::Value UserList::UpdateUserInfo(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateUserInfo(updatejson);
}

// 获取用户信息以提供修改
Json::Value UserList::SelectUserUpdateInfo(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 管理员分页查看用户列表
Json::Value UserList::SelectUserSetInfo(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectUserSetInfo(queryjson);
}

// 删除用户（Token鉴权实现）
Json::Value UserList::DeleteUser(Json::Value &deletejson)
{
    Json::Value json = MoDB::GetInstance()->DeleteUser(deletejson);
    if (json["Result"].asString() == "Success")
    {
        int64_t id = stoll(deletejson["UserId"].asString());
        UserAuthorityMap.erase(id);
    }
    return json;
}

// 更新用户题目信息
bool UserList::UpdateUserProblemInfo(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateUserProblemInfo(updatejson);
}

// 获取用户 Rank 排序
Json::Value UserList::SelectUserRank(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectUserRank(queryjson);
}

// 登录用户通过Token
Json::Value UserList::LoginUserByToken(Json::Value &loginjson)
{
    string token = loginjson["Token"].asString();

    // 根据Token查询UserId
    string userid = ReDB::GetInstance()->GetUserIdByToken(token);

    cout << "UserId: " << userid << endl;

    loginjson["UserId"] = userid;

    return MoDB::GetInstance()->LoginUserByToken(loginjson);
}

// 初始化用户权限
bool UserList::InitUserAuthority()
{
    Json::Value json = MoDB::GetInstance()->SelectUserAuthority();
    if (json["Result"] == "Fail")
        return false;

    for (auto info : json["ArrayInfo"])
    {
        UserAuthorityMap[info["_id"].asInt64()] = info["Authority"].asInt();
    }

    return true;
}

// 将json 的 Token 转化为 VerifyId
void TokenToVerifyId(Json::Value &json)
{
    if (!json["Token"].isNull())
    {
        json["VerifyId"] = ReDB::GetInstance()->GetUserIdByToken(json["Token"].asString());
    }
}

// 获取用户权限
int UserList::GetUserAuthority(Json::Value &json)
{
    /*
        用户权限
        1：游客
        3：普通用户
        5：管理员
    */
    // 如果未发现 ID
    if (json["VerifyId"].isNull())
        return 1;
    try
    {
        int64_t id = stoll(json["VerifyId"].asString());
        // 如果未查询到该用户ID或者用户ID为0是游客
        if (UserAuthorityMap[id] == 0)
            return 1;
        return UserAuthorityMap[id];
    }
    catch (const std::exception &e)
    {
        return 1;
    }
}

// 是否是普通用户或以上
bool UserList::IsOrdinaryUser(Json::Value &json)
{
    TokenToVerifyId(json);
    if (GetUserAuthority(json) >= 3)
        return true;
    else
        return false;
}

// 是否是作者本人或以上
bool UserList::IsAuthor(Json::Value &json)
{
    TokenToVerifyId(json);
    int authority = GetUserAuthority(json);

    if (authority < 3)
        return false;

    if (authority >= 5)
        return true;
    try
    {
        int64_t verifyid = stoll(json["VerifyId"].asString());
        int64_t userid = stoll(json["UserId"].asString());

        if (verifyid == userid)
            return true;
        else
            return false;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 是否是管理员或以上
bool UserList::IsAdministrator(Json::Value &json)
{
    TokenToVerifyId(json);
    if (GetUserAuthority(json) >= 5)
        return true;
    else
        return false;
}

UserList *UserList::GetInstance()
{
    static UserList userlist;
    return &userlist;
}

UserList::UserList()
{
}

UserList::~UserList()
{
}
