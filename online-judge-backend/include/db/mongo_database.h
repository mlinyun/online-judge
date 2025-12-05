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
    mongocxx::instance instance;                  // 该实例应在程序生命周期内存在一次性
    mongocxx::uri uri{constants::db::MONGO_URI};  // 连接字符串
    mongocxx::pool pool{uri};                     // 连接池

    MoDB();

    ~MoDB();

public:
    // 局部静态特性的方式实现单实例模式
    static MoDB *GetInstance();

    // ------------------------------ 用户模块 Start ------------------------------
    /**
     * 功能：用户注册
     * 权限：所有用户均可以注册
     */
    Json::Value UserRegister(Json::Value &registerjson);

    /**
     * 功能：用户登录
     * 权限：所有用户均可以登录（被封禁的除外：未实现）
     */
    Json::Value UserLogin(Json::Value &loginjson);

    /**
     * 功能：查询用户信息
     * 权限：只允许用户本人查询
     */
    Json::Value SelectUserInfo(Json::Value &queryjson);

    /**
     * 功能：查询用户信息（在设置页面修改用户时使用）
     * 权限：只允许用户本人查询
     */
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    /**
     * 功能：更新用户信息
     * 权限：只允许用户本人或者管理员修改
     */
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    /**
     * 功能：删除用户
     * 权限：只允许管理员删除
     */
    Json::Value DeleteUser(Json::Value &deletejson);

    /**
     * 功能：用户排名查询
     * 权限：所有用户均可查询
     */
    Json::Value SelectUserRank(Json::Value &queryjson);

    /**
     * 功能：分页查询用户列表
     * 权限：只允许管理员查询
     */
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    /**
     * 功能：更新用户题目信息（用于用户提交代码后更新题目完成情况）
     * 传入：Json(UserId, ProblemId, Status)
     * 传出：bool(如果题目第一次 AC 则返回 true，否则返回 false)
     */
    bool UpdateUserProblemInfo(Json::Value &updatejson);

    /**
     * 功能：通过 UserId 获取用户名 NickName
     * 传入：string(UserId)
     * 传出：string(NickName)
     */
    std::string GetNickNameByUserId(const std::string &userid);
    // ------------------------------ 用户模块 End ------------------------------

    // ------------------------------ 题目模块 Start ------------------------------
    /**
     * 功能：查询题目信息（单条）
     * 传入：Json(ProblemId)
     * Json(Result, Reason, _id, Title,Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum, ACNum, UserNickName,
     * Tags)
     */
    Json::Value SelectProblemInfo(Json::Value &queryjson);

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
     * 功能：查询公告的详细信息，主要是编辑时的查询（管理员权限）
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

    /**
     * 功能：获取讨论的作者 UserId
     * 传入：讨论 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetDiscussAuthorId(int64_t discussId);
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

    /**
     * 功能：获取题解的作者 UserId
     * 传入：题解 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetSolutionAuthorId(int64_t solutionId);
    // ------------------------------ 题解模块 End ------------------------------

    // ------------------------------ 评论模块 Start ------------------------------
    /**
     * 功能：根据文章类型更新对应的评论数
     * 传入：articleId 文章ID，articleType 文章类型（Discuss/Solution/Announcement），num 评论数变化量
     * 传出：bool 更新是否成功
     */
    bool UpdateArticleComments(int64_t articleId, const std::string &articleType, int num);

    /**
     * 功能：插入父评论
     * 传入：Json(ParentId, Content, UserId)
     * 传出：Json(_id, CreateTime)
     */
    Json::Value InsertFatherComment(Json::Value &insertjson);

    /**
     * 功能：插入子评论
     * 传入：Json(ParentId, Content, UserId)
     * 传出：Json(_id, CreateTime)
     */
    Json::Value InsertSonComment(Json::Value &insertjson);

    /**
     * 功能：管理员查询评论
     * 传入：Json(Page, PageSize)
     * 传出：Json(_id, ParentId, ParentType, Content, CreateTime, Child_Comments._id, Child_Comments.Content,
     * Child_Comments.CreateTime)
     */
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    /**
     * 功能：查询父评论
     * 传入：Json(ParentId, Skip, Limie, SonNum)
     * 传出：Json(ParentId, Content, Likes, CreateTime, Child_Total, User(Avatar, NickName),
     * Child_Comments(_id, Content, Likes, CreateTime, User(Avatar, NickName)))
     */
    Json::Value GetFatherComment(Json::Value &queryjson);

    /**
     * 功能：获取子评论
     * 传入：Json(ParentId, Skip, Limit)
     * 传出：Json(Child_Total, Child_Comments(_id, Content, Likes, CreateTime, User(NickName, Avatar)))
     */
    Json::Value GetSonComment(Json::Value &queryjson);

    /**
     * 功能：删除某一篇文章（讨论，题解，公告）的所有文章，主要服务于删除文章
     * 传入：Json(ArticleId)
     * 传出：bool
     */
    bool DeleteArticleComment(Json::Value &deletejson);

    /**
     * 功能：删除父评论
     * 传入：Json(CommentId)
     * 传出：Json(Result, Reason, DeleteNum)
     */
    Json::Value DeleteFatherComment(Json::Value &deletejson);

    /**
     * 功能：删除子评论
     * 传入：Json(CommentId)
     * 传出：Json(Result, Reason, DeleteNum)
     */
    Json::Value DeleteSonComment(Json::Value &deletejson);

    /**
     * 功能：获取评论的作者 UserId
     * 传入：评论 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetCommentAuthorId(int64_t commentId);
    // ------------------------------ 评论模块 End ------------------------------

    // ------------------------------ 测评记录模块 Start ------------------------------
    /**
     * 功能：插入待测评记录
     * 传入：Json(ProblemId, UserId, UserNickName, ProblemTitle, Language, Code)
     * 传出：SubmitId 测评的 ID
     */
    std::string InsertStatusRecord(Json::Value &insertjson);

    /**
     * 功能：更新测评记录
     * 传入：Json(SubmitId, Status, RunTime, RunMemory, Length, CompilerInfo, TestInfo[(Status, StandardInput,
     * StandardOutput, PersonalOutput, RunTime, RunMemory)])
     * 传出：bool
     */
    bool UpdateStatusRecord(Json::Value &updatejson);

    /**
     * 功能：分页查询测评记录
     * 传入：Json(SearchInfo, PageSize, Page)
     * 传出：测评全部信息，详情请见 MongoDB 集合表
     */
    Json::Value SelectStatusRecordList(Json::Value &queryjson);

    /**
     * 功能：查询测评记录
     * 传入：Json(SubmitId)
     * 传出：全部记录，详情请看 MongoDB 集合表
     */
    Json::Value SelectStatusRecord(Json::Value &queryjson);

    /**
     * 功能：获取状态记录的作者 UserId
     * 传入：状态记录 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetStatusRecordAuthorId(int64_t statusRecordId);
    // ------------------------------ 测评记录模块 End ------------------------------

    // ------------------------------ Token 鉴权实现 Start ------------------------------
    /**
     * 功能：用户登录通过 Token 鉴权
     * 传入：Json(UserId)
     * 传出：Json(Result, Reason, Info(_id, NickName, Avatar, CommentLikes, Solves, Authority))
     */
    Json::Value LoginUserByToken(Json::Value &loginjson);

    /**
     * 功能：查询所有用户的权限
     * 传入：void
     * 传出：Json(Result, _id, Authority)
     */
    Json::Value SelectUserAuthority();
    // ------------------------------ Token 鉴权实现 End ------------------------------
};

#endif  // MONGO_DATABASE_H