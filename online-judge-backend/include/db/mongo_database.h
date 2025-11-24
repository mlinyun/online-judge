#ifndef MONGO_DATABASE_H
#define MONGO_DATABASE_H

#include <json/json.h>

#include <atomic>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "constants/db.h"

using namespace std;

/**
 * MongoDB 数据库操作类头文件
 */
class MoDB {
private:
    static mongocxx::instance instance;           // 该实例应在程序生命周期内存在一次性
    mongocxx::uri uri{constants::db::MONGO_URI};  // 连接字符串
    mongocxx::pool pool{uri};                     // 连接池

    atomic_int64_t m_problem_id;        // 题目 ID 的最大值
    atomic_int64_t m_status_record_id;  // 提交记录 ID 的最大值
    atomic_int64_t m_comment_id;        // 评论 ID 的最大值
    atomic_int64_t m_article_id;        // 文章 ID 的最大值

    /**
     * 功能：获取指定集合的最大 ID
     * 参数：name 集合名称
     * 返回值：最大 ID
     */
    int64_t GetMaxId(string name);

    MoDB();

    ~MoDB();

public:
    // 局部静态特性的方式实现单实例模式
    static MoDB *GetInstance();

    /**
     * 功能：用户注册
     * 传入：Json(NickName, Account, PassWord, PersonalProfile, School, Major)
     * 传出：Json(Result, Reason)
     */
    Json::Value RegisterUser(Json::Value &registerjson);

    /**
     * 功能：用户登录
     * 传入：Json(Account, PassWord)
     * 传出：Json(Result, Reason, Info(_id, NickName, Avatar, CommentLikes, Solves, Authority))
     */
    Json::Value LoginUser(Json::Value &loginjson);

    /**
     * 功能：查询用户信息
     * 传入：Json(UserId)
     * 传出：Json(Result, Reason, _id, Avatar, NickName, PersonalProfile, School, Major, JoinTime, Solves, ACNum,
     * SubmitNum)
     */
    Json::Value SelectUserInfo(Json::Value &queryjson);

    /**
     * 功能：查询用户信息（在设置页面修改用户时使用）
     * 传入：Json(UserId)
     * 传出：Json(_id, Avatar, NickName, PersonalProfile, School, Major)
     */
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    /**
     * 功能：更新用户信息
     * 传入：Json(UserId, Avatar, NickName, PersonalProfile, School, Major)
     * 传出：Json(Result, Reason)
     */
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    /**
     * 功能：删除用户
     * 传入：Json(UserId)
     * 传出：Json(Result, Reason)
     */
    Json::Value DeleteUser(Json::Value &deletejson);

    /**
     * 功能：用户排名查询
     * 传入：Json(Page, PageSize)
     * 传出：Json(ArrayInfo[_id, Rank, Avatar, NickName, PersonalProfile, SubmitNum, ACNum], TotalNum)
     */
    Json::Value SelectUserRank(Json::Value &queryjson);

    /**
     * 功能：分页查询用户列表（管理员权限）
     * 传入：Json(Page, PageSize)
     * Json(Result, Reason, ArrayInfo[_id, NickName, PersonalProfile, School, Major, JoinTime], TotalNum)
     */
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    /**
     * 功能：更新用户题目信息（用于用户提交代码后更新题目完成情况）
     * 传入：Json(UserId, ProblemId, Status)
     * 传出：bool(如果题目第一次 AC 则返回 true，否则返回 false)
     */
    bool UpdateUserProblemInfo(Json::Value &updatejson);
};

#endif  // MONGO_DATABASE_H