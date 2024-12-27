#include "UserList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>

using namespace std;

// 注册用户
Json::Value UserList::RegisterUser(Json::Value &registerjson)
{
    return MoDB::GetInstance()->RegisterUser(registerjson);
}

// 登录用户
Json::Value UserList::LoginUser(Json::Value &loginjson)
{
    return MoDB::GetInstance()->LoginUser(loginjson);
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

// 删除用户
Json::Value UserList::DeleteUser(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteUser(deletejson);
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