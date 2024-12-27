#ifndef MONGODATABASE_H
#define MONGODATABASE_H

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <jsoncpp/json/json.h>
#include <atomic>

// MongoDB 数据库
class MoDB
{
public:
    // 局部静态特性的方式实现单实例
    static MoDB *GetInstance();

    // ++++++++++++++++++++++++++++++ 用户表 User Start ++++++++++++++++++++++++++++++
    /*
        功能：用户注册
        传入：Json(NickName, Account, PassWord, PersonalProfile, School, Major)
        传出：Json(Result, Reason)
    */
    Json::Value RegisterUser(Json::Value &registerjson);

    /*
        功能：用户登录
        传入：Json(Account, PassWord)
        传出：Json(Result, Reason, Info(_id, NickName, Avatar, CommentLikes, Solves, Authority))
    */
    Json::Value LoginUser(Json::Value &loginjson);

    /*
        功能：用户主页展示
        传入：Json(UserId)
        传出：Json(Result, Reason, _id, Avatar, NickName, PersonalProfile, School, Major, JoinTime, Solves, ACNum, SubmitNum)
    */
    Json::Value SelectUserInfo(Json::Value &queryjson);

    /*
        功能：更改用户信息
        传入：Json(UserId, Avatar, PersonalProfile, School, Major)
        传出：Json(Result, Reason)
    */
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    /*
        功能：查询用户表，用于设置页面修改用户
        传入：Json(UserId)
        传出：Json(_id, Avatar, NickName, PersonalProfile, School, Major)
    */
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    /*
        功能：分页查询用户列表
        传入：Json(Page,PageSize)
        传出：Json(_id, NickName, PersonalProfile, School, Major, JoinTime, TotalNum)
    */
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    /*
        功能：删除用户
        传入：Json(UserId)
        传出：Json(Result, Reason)
    */
    Json::Value DeleteUser(Json::Value &deletejson);

    /*
        功能：更新用户的题目信息（当用户提交代码时）
        传入：Json(UserId, ProblemId, Status)
        传出：bool(如果 AC 是否向其添加)
    */
    bool UpdateUserProblemInfo(Json::Value &updatejson);

    /*
        功能：获取用户的 Rank 排名
        传入：Json(Page, PageSize)
        传出：Json(ArrayInfo[_id, Rank, Avatar, NickName, PersonalProfile, SubmitNum, ACNum], TotalNum)
    */
    Json::Value SelectUserRank(Json::Value &queryjson);
    // ++++++++++++++++++++++++++++++ 用户表 User End ++++++++++++++++++++++++++++++

private:
    /*
        功能：获取某一个集合中最大的 ID
    */
    int64_t GetMaxId(std::string name);

    MoDB();

    ~MoDB();

private:
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri{};           // 连接配置
    mongocxx::pool pool{uri};      // 连接池

    std::atomic_int64_t m_problemid;      // 题目 ID
    std::atomic_int64_t m_statusrecordid; // 测评 ID
    std::atomic_int64_t m_commentid;      // 评论 ID
    std::atomic_int64_t m_articleid;      // 文章 ID
};

#endif
