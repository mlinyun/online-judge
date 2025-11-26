#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include <json/json.h>

#include <unordered_map>

/**
 * 用户服务类头文件
 */
class UserService {
private:
    // 用户权限的哈希表，键：用户ID，值：用户权限
    std::unordered_map<int64_t, int> UserAuthorityMap;

    UserService();

    ~UserService();

public:
    // 局部静态特性的方式实现单实例模式
    static UserService *GetInstance();

    // 用户注册
    Json::Value RegisterUser(Json::Value &registerjson);

    // 用户登录
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

    // 更新用户题目信息（用于用户提交代码后更新题目完成情况）
    bool UpdateUserProblemInfo(Json::Value &updatejson);

    // -------------------- Token 鉴权实现 Start --------------------
    // 登录用户通过Token
    Json::Value LoginUserByToken(Json::Value &loginjson);

    // 初始化用户权限
    bool InitUserAuthority();

    // 获取用户权限
    int GetUserAuthority(Json::Value &json);

    // 权限是否是普通用户或以上
    bool IsOrdinaryUser(Json::Value &json);

    // 权限是否是作者本人或以上
    bool IsAuthor(Json::Value &json);

    // 权限是否是管理员或以上
    bool IsAdministrator(Json::Value &json);
    // -------------------- Token 鉴权实现 End --------------------
};

#endif  // USER_SERVICE_H