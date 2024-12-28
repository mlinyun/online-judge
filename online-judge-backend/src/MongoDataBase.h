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

    // ++++++++++++++++++++++++++++++ 用户 User Start ++++++++++++++++++++++++++++++
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
        传入：Json(Page, PageSize)
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
    // ++++++++++++++++++++++++++++++ 用户 User End ++++++++++++++++++++++++++++++

    // ++++++++++++++++++++++++++++++ 题目 Problem Start ++++++++++++++++++++++++++++++
    /*
        功能：管理员查询题目详细信息
        传入：Json(ProblemId)
        传出：Json(Result, Reason,_id, Title, Description, TimeLimit, MemoryLimit, UserNickName, JudgeNum, Tags)
    */
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    /*
        功能：用户查询题目详细信息
        传入：Json(ProblemId)
        传出：Json(Result, Reason, _id, Title,Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum, ACNum, UserNickName, Tags)
    */
    Json::Value SelectProblem(Json::Value &queryjson);
    /*
        功能：插入题目
        传入：Json(Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
        传出：Json(Reuslt, Reason, ProblemId)
    */
    Json::Value InsertProblem(Json::Value &insertjson);

    /*
        功能：修改题目信息
        传入：Json(ProblemId, Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
        传出：Json(Result, Reason)
    */
    Json::Value UpdateProblem(Json::Value &updatejson);

    /*
        功能：删除题目
        传入：Json(ProblemId)
        传出：Json(Result, Reason)
    */
    Json::Value DeleteProblem(Json::Value &deletejson);

    /*
        功能：分页获取题目列表（包含查询条件，暂时未添加）
        传入：Json(SearchInfo{Id, Title, Tags[]}, Page, PageSize)
        传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum, TLENum, MLENum, SENum, Tags]), TotalNum)
    */
    Json::Value SelectProblemList(Json::Value &queryjson);

    /*
        功能：管理员分页获取题目列表
        传入：Json(Page, PageSize)
        传出：Json(ArrayInfo([ProblemId, Title, SubmitNum, CENum, ACNum, WANum, TLENum, MLENum, SENum, Tags]), TotalNum)
    */
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    /*
        功能：更新题目的状态数量
        传入：Json(ProblemId, Status)
        传出：bool
    */
    bool UpdateProblemStatusNum(Json::Value &updatejson);

    /*
        功能：获取题目的所有标签
        传入：void
        传出：Json(tags)
    */
    Json::Value getProblemTags();
    // ++++++++++++++++++++++++++++++ 题目 Problem End ++++++++++++++++++++++++++++++

    // ++++++++++++++++++++++++++++++ 公告 Announcement Start ++++++++++++++++++++++++++++++
    /*
        功能：添加公告
        传入：Json(Title, Content, UserId, Level)
        传出：Json(Result, Reason)
    */
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    /*
        功能：分页查询公告
        传入：Json(Page, PageSize)
        传出：Json([Result, Reason, _id, Title, Views, Comments, CreateTime], TotalNum)
    */
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    /*
        功能：查询公告的详细信息，主要是编辑时的查询
        传入：Json(AnnouncementId)
        传出：Json(Result, Reason, Title, Content, Level)
    */
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    /*
        功能：查询公告的详细内容，并且将其浏览量加一
        传入：Json(AnnouncementId)
        传出：Json(Title, Content, Views, Comments, CreateTime, UpdateTime)
    */
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    /*
        功能：修改公告的评论数
        传入：Json(ArticleId, Num)
        传出：bool
    */
    bool UpdateAnnouncementComments(Json::Value &updatejson);

    /*
        功能：更新公告
        传入：Json(AnnouncementId, Title, Content, Level)
        传出；Json(Result, Reason)
    */
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    /*
        功能：删除公告
        传入：Json(AnnouncementId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteAnnouncement(Json::Value &deletejson);
    // ++++++++++++++++++++++++++++++ 公告 Announcement End ++++++++++++++++++++++++++++++

    // ++++++++++++++++++++++++++++++ 讨论 Disscuss Start ++++++++++++++++++++++++++++++
    /*
        功能：添加讨论
        传入：Json(Title, Content, ParentId, UserId) 如果是父讨论 ParentId = 0
        传出：Json(Result)
    */
    Json::Value InsertDiscuss(Json::Value &insertjson);

    /*
        功能：分页查询讨论
        传入：Json(SearchInfo, Page, PageSize)
        传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
    */
    Json::Value SelectDiscussList(Json::Value &queryjson);

    /*
        功能：管理员分页查询讨论
        传入：Json(Page,PageSize)
        传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
    */
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    /*
        功能：查询讨论的详细信息，主要是编辑时的查询
        传入：Json(DiscussId)
        传出：Json(Result, Reason, Title, Content)
    */
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    /*
        功能：查询讨论的详细内容，并且将其浏览量加一
        传入：Json(DiscussId)
        传出：Json(Resutl, Reason, Content, Views, Comments, CreateTime, UpdateTime, User.NickName, User.Avatar)
    */
    Json::Value SelectDiscuss(Json::Value &queryjson);

    /*
        功能：修改讨论的评论数
        传入：Json(DiscussId, Num)
        传出：bool
    */
    bool UpdateDiscussComments(Json::Value &updatejson);

    /*
        功能：更新讨论
        传入：Json(DiscussId, Title, Content)
        传出；Json(Result,Reason)
    */
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    /*
        功能：删除讨论
        传入：Json(DiscussId)
        传出：Json(Result, Reason)
    */
    Json::Value DeleteDiscuss(Json::Value &deletejson);
    // ++++++++++++++++++++++++++++++ 讨论 Disscuss End ++++++++++++++++++++++++++++++

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
