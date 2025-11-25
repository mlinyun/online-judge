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
    atomic_int64_t m_announcement_id;   // 公告 ID 的最大值
    atomic_int64_t m_discussion_id;     // 讨论 ID 的最大值
    atomic_int64_t m_solution_id;       // 题解 ID 的最大值
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

    // ------------------------------ 用户模块 Start ------------------------------
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
    // ------------------------------ 用户模块 End ------------------------------

    // ------------------------------ 题目模块 Start ------------------------------
    /**
     * 功能：查询题目信息（单条）
     * 传入：Json(ProblemId)
     * Json(Result, Reason, _id, Title,Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum, ACNum, UserNickName,
     * Tags)
     */
    Json::Value SelectProblem(Json::Value &queryjson);

    /**
     * 功能：查询题目信息（管理员权限）
     * 传入：Json(ProblemId)
     * 传出：Json(Result, Reason,_id, Title, Description, TimeLimit, MemoryLimit, UserNickName, JudgeNum, Tags)
     */
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    /**
     * 功能：插入题目（管理员权限）
     * 传入：Json(Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
     * 传出：Json(Result, Reason, ProblemId)
     */
    Json::Value InsertProblem(Json::Value &insertjson);

    /**
     * 功能：更新题目信息（管理员权限）
     * 传入：Json(ProblemId, Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
     * 传出：Json(Result, Reason)
     */
    Json::Value UpdateProblem(Json::Value &updatejson);

    /**
     * 功能：删除题目（管理员权限）
     * 传入：Json(ProblemId)
     * 传出：Json(Result, Reason)
     */
    Json::Value DeleteProblem(Json::Value &deletejson);

    /**
     * 功能：分页获取题目列表
     * 传入：Json(Page, PageSize, SearchInfo{Id, Title, Tags[]})
     * 传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum,
     * TLENum, MLENum, SENum, Tags]), TotalNum)
     */
    Json::Value SelectProblemList(Json::Value &queryjson);

    /**
     * 功能：分页获取题目列表（管理员权限）
     * 传入：Json(Page, PageSize)
     * 传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum,
     * TLENum, MLENum, SENum, Tags]), TotalNum)
     */
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    /**
     * 功能：更新题目的状态数量
     * 传入：Json(ProblemId, Status)
     * 传出：bool
     */
    bool UpdateProblemStatusNum(Json::Value &updatejson);
    // ------------------------------ 题目模块 End ------------------------------

    // ------------------------------ 标签模块 Start ------------------------------
    /**
     * 功能：获取题目的所有标签
     * 传入：void
     * 传出：Json(Tags)
     */
    Json::Value GetProblemTags();
    // ------------------------------ 标签模块 End ------------------------------

    // ------------------------------ 公告模块 Start ------------------------------
    /**
     * 功能：添加公告（管理员权限）
     * 传入：Json(Title, Content, UserId, Level)
     * 传出：Json(Result, Reason)
     */
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    /**
     * 功能：查询公告详细信息，并将其浏览量加 1
     * 传入：Json(AnnouncementId)
     * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime], TotalNum)
     */
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    /**
     * 功能：更新公告（管理员权限）
     * 传入：Json(AnnouncementId, Title, Content, Level)
     * 传出；Json(Result, Reason)
     */
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    /**
     * 功能：删除公告（管理员权限）
     * 传入：Json(AnnouncementId)
     * 传出：Json(Result,Reason)
     */
    Json::Value DeleteAnnouncement(Json::Value &deletejson);

    /**
     * 功能：分页查询公告列表
     * 传入：Json(Page, PageSize)
     * 传出：Json([Result, Reason, _id, Title, Views, Comments, CreateTime], TotalNum)
     */
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    /**
     * 功能：查询公告的详细信息，主要是编辑时的查询
     * 传入：Json(AnnouncementId)
     * 传出：Json(Result, Reason, Title, Content, Level)
     */
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    /**
     * 功能：更新公告的评论数量
     * 传入：Json(ArticleId, Num)
     * 传出：bool
     */
    bool UpdateAnnouncementComments(Json::Value &updatejson);
    // ------------------------------ 公告模块 End ------------------------------

    // ------------------------------ 讨论模块 Start ------------------------------
    /**
     * 功能：添加讨论
     * 传入：Json(Title, Content, ParentId, UserId) 如果是父讨论ParentId=0
     * 传出：Json(Result)
     */
    Json::Value InsertDiscuss(Json::Value &insertjson);

    /**
     * 功能：查询讨论的详细内容，并且将其浏览量加 1
     * 传入：Json(DiscussId)
     * 传出：Json(Result, Reason, _id, Title, Content, Views, Comments, CreateTime, UpdateTime, User.NickName,
     * User.Avatar)
     */
    Json::Value SelectDiscuss(Json::Value &queryjson);

    /**
     * 功能：更新讨论
     * 传入：Json(DiscussId, Title, Content)
     * 传出；Json(Result,Reason)
     */
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    /**
     * 功能：删除讨论
     * 传入：Json(DiscussId)
     * 传出：Json(Result, Reason)
     */
    Json::Value DeleteDiscuss(Json::Value &deletejson);

    /**
     * 功能：分页查询讨论
     * 传入：Json(SearchInfo, Page, PageSize)
     * 传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
     */
    Json::Value SelectDiscussList(Json::Value &queryjson);

    /**
     * 功能：分页查询讨论（管理员权限）
     * 传入：Json(Page,PageSize)
     * 传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
     */
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    /**
     * 功能：查询讨论的详细信息，主要是编辑时的查询
     * 传入：Json(DiscussId)
     * 传出：Json(Result, Reason, Title, Content)
     */
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    /**
     * 功能：更新讨论的评论数量
     * 传入：Json(DiscussId, Num)
     * 传出：bool
     */
    bool UpdateDiscussComments(Json::Value &updatejson);
    // ------------------------------ 讨论模块 End ------------------------------

    // ------------------------------ 题解模块 Start ------------------------------
    /**
     * 功能：添加题解
     * 传入：Json(Title, Content, ParentId, UserId, Public)
     * 传出：Json(Result, Reason)
     */
    Json::Value InsertSolution(Json::Value &insertjson);

    /**
     * 功能：查询题解的详细内容，并且将其浏览量加 1
     * 传入：Json(SolutionId)
     * 传出：Json(Result, Reason, Title, Content, Views, Comments, CreateTime, UpdateTime, User.NickName, User.Avatar)
     */
    Json::Value SelectSolution(Json::Value &queryjson);

    /**
     * 功能：更新题解
     * 传入：Json(SolutionId, Title, Content, Public)
     * 传出；Json(Result, Reason)
     */
    Json::Value UpdateSolution(Json::Value &updatejson);

    /**
     * 功能：删除题解
     * 传入：Json(SolutionId)
     * 传出：Json(Result, Reason)
     */
    Json::Value DeleteSolution(Json::Value &deletejson);

    /**
     * 功能：分页查询题解（公开题解）
     * 传入：Json(SearchInfo(ParentId, UserId), Page, PageSize)
     * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName],
     * TotalNum)
     */
    Json::Value SelectSolutionList(Json::Value &queryjson);

    /**
     * 功能：分页查询题解（管理员权限）
     * 传入：Json(Page, PageSize)
     * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName],
     * TotalNum)
     */
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);

    /**
     * 功能：查询题解的详细信息，主要是编辑时的查询
     * 传入：Json(SolutionId)
     * 传出：Json(Result, Reason, Title, Content, UserId, Public)
     */
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);

    /**
     * 功能：修改题解的评论数
     * 传入：Json(ArticleId, Num)
     * 传出：bool
     */
    bool UpdateSolutionComments(Json::Value &updatejson);
    // ------------------------------ 题解模块 End ------------------------------
};

#endif  // MONGO_DATABASE_H