#include "services/user_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
UserService *UserService::GetInstance() {
    static UserService user_service;
    return &user_service;
}

// 用户注册
Json::Value UserService::RegisterUser(Json::Value &registerjson) {
    return MoDB::GetInstance()->RegisterUser(registerjson);
}

// 用户登录
Json::Value UserService::LoginUser(Json::Value &loginjson) {
    return MoDB::GetInstance()->LoginUser(loginjson);
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

// 删除用户
Json::Value UserService::DeleteUser(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteUser(deletejson);
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

UserService::UserService() {
    // 构造函数实现
}

UserService::~UserService() {
    // 析构函数实现
}