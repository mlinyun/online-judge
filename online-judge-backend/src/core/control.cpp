#include "core/control.h"

#include <iostream>

#include "judger/judger.h"
#include "services/announcement_service.h"
#include "services/comment_service.h"
#include "services/discuss_service.h"
#include "services/problem_service.h"
#include "services/solution_service.h"
#include "services/status_record_service.h"
#include "services/tag_service.h"
#include "services/user_service.h"

using namespace std;

// 用户注册
Json::Value Control::RegisterUser(Json::Value &registerjson) {
    return UserService::GetInstance()->RegisterUser(registerjson);
}

// 用户登录
Json::Value Control::LoginUser(Json::Value &loginjson) {
    return UserService::GetInstance()->LoginUser(loginjson);
}

// 查询用户信息
Json::Value Control::SelectUserInfo(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserInfo(queryjson);
}

// 查询用户信息（在设置页面修改用户时使用）
Json::Value Control::SelectUserUpdateInfo(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 更新用户信息
Json::Value Control::UpdateUserInfo(Json::Value &updatejson) {
    return UserService::GetInstance()->UpdateUserInfo(updatejson);
}

// 删除用户
Json::Value Control::DeleteUser(Json::Value &deletejson) {
    return UserService::GetInstance()->DeleteUser(deletejson);
}

// 用户排名查询
Json::Value Control::SelectUserRank(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserRank(queryjson);
}

// 分页查询用户列表（管理员权限）
Json::Value Control::SelectUserSetInfo(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserSetInfo(queryjson);
}

Control::Control() {
    // 构造函数实现
}

Control::~Control() {
    // 析构函数实现
}