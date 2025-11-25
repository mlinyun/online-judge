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
    // ------------------------------ 用户模块 Start ------------------------------
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
    // ------------------------------ 用户模块 End ------------------------------

    // ------------------------------ 题目模块 Start ------------------------------
    // 查询题目信息（单条）
    Json::Value SelectProblem(Json::Value &queryjson);

    // 查询题目信息（管理员权限）
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    // 编辑题目：包含插入和更新题目（管理员权限）
    Json::Value EditProblem(Json::Value &insertjson);

    // 删除题目（管理员权限）
    Json::Value DeleteProblem(Json::Value &deletejson);

    // 分页获取题目列表
    Json::Value SelectProblemList(Json::Value &queryjson);

    // 分页获取题目列表（管理员权限）
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);
    // ------------------------------ 题目模块 End ------------------------------

    // ------------------------------ 标签模块 Start ------------------------------
    // 获取题目的所有标签
    Json::Value GetTags(Json::Value &queryjson);
    // ------------------------------ 标签模块 End ------------------------------

    Control();

    ~Control();
};

#endif  // CONTROL_H