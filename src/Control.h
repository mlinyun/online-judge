#ifndef CONTROL_H
#define CONTROL_H

#include <jsoncpp/json/json.h>
#include <string>

class Control {
public:

    // ----------------用户表 User-----------------
    // 注册用户
    Json::Value RegisterUser(Json::Value &registerjson);

    // 登录用户
    Json::Value LoginUser(Json::Value &loginjson);

    // 获取用户Rank排名
    Json::Value SelectUserRank(Json::Value &queryjson);

    // 获取用户大部分信息
    Json::Value SelectUserInfo(Json::Value &queryjson);

    // 更新用户信息
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    // 获取用户信息，以供修改
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    // 分页获取用户信息
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    // 删除用户
    Json::Value DeleteUser(Json::Value &deletejson);

    Control();

    ~Control();

private:
};

#endif