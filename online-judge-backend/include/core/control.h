#ifndef CONTROL_H
#define CONTROL_H

#include <json/json.h>

/**
 * 控制类头文件
 */
class Control {
private:
    /* data */
public:
    // 注册用户
    Json::Value RegisterUser(Json::Value &registerjson);

    // 登录用户
    Json::Value LoginUser(Json::Value &loginjson);

    // 查询用户信息
    Json::Value SelectUserInfo(Json::Value &queryjson);

    // 查询用户信息（在设置页面修改用户时使用）
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    // 更新用户信息
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    // 删除用户
    Json::Value DeleteUser(Json::Value &deletejson);

    // 用户排名查询
    Json::Value SelectUserRank(Json::Value &queryjson);

    // 分页查询用户列表（管理员权限）
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    Control();

    ~Control();
};

#endif  // CONTROL_H