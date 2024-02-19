#ifndef MONGODATABASE_H
#define MONGODATABASE_H

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <jsoncpp/json/json.h>
#include <atomic>

class MoDB {
public:
    // 局部静态特性的方式实现单实例
    static MoDB *GetInstance();

    // +++++++++++++++++++++++用户表User+++++++++++++++++++++++++++++
    /*
        功能：用户注册
        传入：Json(NickName,Account,PassWord,PersonalProfile,School,Major)
        传出：Json(Result,Reason)
    */
    Json::Value RegisterUser(Json::Value &registerjson);

    /*
        功能：用户登录
        传入：Json(Account,PassWord)
        传出：Json(Result,Reason,Info(_id,NickName,Avatar,CommentLikes,Solves,Authority))
    */
    Json::Value LoginUser(Json::Value &loginjson);

    /*
        功能：更新用户的题目信息，当用户提交代码时
        传入：Json(UserId,ProblemId,Status)
        传出：bool(如果AC是否向其添加)
    */
    bool UpdateUserProblemInfo(Json::Value &updatejson);

    /*
        功能：获取用户的Rank排名
        传入：Json(Page,PageSize)
        传出：Json(ArrayInfo[_id,Rank,Avatar,NickName,PersonalProfile,SubmitNum,ACNum],TotalNum)
    */
    Json::Value SelectUserRank(Json::Value &queryjson);

    /*
        功能：用户主页展示
        传入：Json(UserId)
        传出：Json(Result,Reason,_id,Avatar,NickName,PersonalProfile,School,Major,JoinTime,Solves,ACNum,SubmitNum)
    */
    Json::Value SelectUserInfo(Json::Value &queryjson);

    /*
        功能：更改用户信息
        传入：Json(UserId,Avatar,PersonalProfile,School,Major)
        传出：Json(Result,Reason)
    */
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    /*
        功能：查询用户表，用于修改用户
        传入：Json(UserId)
        传出：Json(_id,Avatar,NickName,PersonalProfile,School,Major)
    */
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    /*
        功能：分页查询用户列表
        传入：Json(Page,PageSize)
        传出：Json(_id,NickName,PersonalProfile,School,Major,JoinTime,TotalNum)
    */
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    /*
        功能：删除用户
        传入：Json(UserId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteUser(Json::Value &deletejson);

    // ++++++++++++++++++++++++++题目表Problem+++++++++++++++++++++++++++++
    /*
        功能：管理员查询题目信息
        传入：Json(ProblemId)
        传出：Json(Result,Reason,_id,Title,Description,TimeLimit,MemoryLimit,UserNickName,JudgeNum,Tags)
    */
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    /*
        功能：获取题目信息
        传入：Json(ProblemId)
        传出：Json(Result,Reason,_id,Title,Description,TimeLimit,MemoryLimit,JudgeNum,SubmitNum,ACNum,UserNickName,Tags)
    */
    Json::Value SelectProblem(Json::Value &queryjson);

    /*
        功能：插入题目
        传入：Json(Title,Description,TimeLimit,MemoryLimit,JudgeNum,Tags,UseNickName)
        传出：Json(Reuslt,Reason,ProblemId)
    */
    Json::Value InsertProblem(Json::Value &insertjson);

    /*
        功能：修改题目信息
        传入：Json(ProblemId,Title,Description,TimeLimit,MemoryLimit,JudgeNum,Tags,UseNickName)
        传出：Json(Result,Reason)
    */
    Json::Value UpdateProblem(Json::Value &updatejson);

    /*
        功能：删除题目
        传入：Json(ProblemId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteProblem(Json::Value &deletejson);

    /*
        功能：分页获取题目列表（包含查询条件，暂时未添加）
        前端传入
        Json(SearchInfo,Page,PageSize,MatchString)
        后端传出
        Json(ProblemId,Title,SubmitNum,CENum,ACNum,WANum,TLENum,MLENum,SENum,Tags),TotalNum
    */
    Json::Value SelectProblemList(Json::Value &queryjson);

    /*
        功能：管理员分页获取题目列表
        传入：Json(Page,PageSize)
        传出：Json(ArrayInfo([ProblemId,Title,SubmitNum,CENum,ACNum,WANum,TLENum,MLENum,SENum,Tags]),TotalNum)
    */
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    /*
        功能：更新题目的状态数量
        传入：Json(ProblemId,Status)
        传出：bool
    */
    bool UpdateProblemStatusNum(Json::Value &updatejson);

    /*
        功能：获取题目的所有标签
        传入：void
        传出：Json(tags)
    */
    Json::Value getProblemTags();

    // +++++++++++++++++++++++++Announcement 公告表++++++++++++++++++++++++++++
    /*
        功能：添加公告
        传入：Json(Title,Content,UserId,Level)
        传出：Json(Result,Reason)
    */
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    /*
        功能：分页查询公告
        传入：Json(Page,PageSize)
        传出：Json([Result,Reason,_id,Title,Views,Comments,CreateTime],TotalNum)
    */
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    /*
        功能：查询公告的详细信息，主要是编辑时的查询
        传入：Json(AnnouncementId)
        传出：Json(Result,Reason,Title,Content,Level)
    */
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    /*
        功能：查询公告的详细内容，并且将其浏览量加一
        传入：Json(AnnouncementId)
        传出：Json(Title,Content,Views,Comments,CreateTime,UpdateTime)
    */
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    /*
        功能：修改公告的评论数
        传入：Json(ArticleId,Num)
        传出：bool
    */
    bool UpdateAnnouncementComments(Json::Value &updatejson);

    /*
        功能：更新公告
        传入：Json(AnnouncementId,Title,Content,Level)
        传出；Json(Result,Reason)
    */
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    /*
        功能：删除公告
        传入：Json(AnnouncementId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteAnnouncement(Json::Value &deletejson);

    // +++++++++++++++++++++++++Disscuss 讨论表++++++++++++++++++++++++++++
    /*
        功能：添加讨论
        传入：Json(Title,Content,ParentId,UserId) 如果是父讨论ParentId=0
        传出：Json(Result)
    */
    Json::Value InsertDiscuss(Json::Value &insertjson);

    /*
        功能：分页查询讨论
        传入：Json(SearchInfo,Page,PageSize)
        传出：Json(_id,Title,Views,Comments,CreateTime,User.Avatar,User.NickName)
    */
    Json::Value SelectDiscussList(Json::Value &queryjson);

    /*
        功能：管理员分页查询讨论
        传入：Json(Page,PageSize)
        传出：Json(_id,Title,Views,Comments,CreateTime,User.Avatar,User.NickName)
    */
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    /*
        功能：查询讨论的详细信息，主要是编辑时的查询
        传入：Json(DiscussId)
        传出：Json(Result,Reason,Title,Content)
    */
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    /*
        功能：查询讨论的详细内容，并且将其浏览量加一
        传入：Json(DiscussId)
        传出：Json(Resutl,Reason,Content,Views,Comments,CreateTime,UpdateTime,User.NickName,User.Avatar)
    */
    Json::Value SelectDiscuss(Json::Value &queryjson);

    /*
        功能：修改讨论的评论数
        传入：Json(DiscussId,Num)
        传出：bool
    */
    bool UpdateDiscussComments(Json::Value &updatejson);

    /*
        功能：更新讨论
        传入：Json(DiscussId,Title,Content)
        传出；Json(Result,Reason)
    */
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    /*
        功能：删除讨论
        传入：Json(DiscussId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteDiscuss(Json::Value &deletejson);

    // +++++++++++++++++++++++++Solution 题解表++++++++++++++++++++++++++++
    /*
        功能：添加题解
        传入：Json(Title,Content,ParentId,UserId,Public)
        传出：Json(Result,Reason)
    */
    Json::Value InsertSolution(Json::Value &insertjson);

    /*
        功能：分页查询题解（公开题解）
        传入：Json(SearchInfo,Page,PageSize)
        传出：Json(_id,Title,Views,Comments,CreateTime,User.Avatar,User.NickName)
    */
    Json::Value SelectSolutionList(Json::Value &queryjson);

    /*
        功能：管理员分页查询题解
        传入：Json(Page,PageSize)
        传出：Json(_id,Title,Views,Comments,CreateTime,User.Avatar,User.NickName)
    */
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);

    /*
        功能：查询题解的详细信息，主要是编辑时的查询
        传入：Json(SolutionId)
        传出：Json(Result,Reason,Title,Content,Public)
    */
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);

    /*
        功能：查询题解的详细内容，并且将其浏览量加一
        传入：Json(SolutionId)
        传出：Json(Result,Reason,Title,Content,Views,Comments,CreateTime,UpdateTime,User.NicaName,User.Avatar)
    */
    Json::Value SelectSolution(Json::Value &queryjson);

    /*
        功能：修改题解的评论数
        传入：Json(ArticleId,Num)
        传出：bool
    */
    bool UpdateSolutionComments(Json::Value &updatejson);

    /*
        功能：更新题解
        传入：Json(SolutionId,Title,Content,Public)
        传出；Json(Result,Reason)
    */
    Json::Value UpdateSolution(Json::Value &updatejson);

    /*
        功能：删除题解
        传入：Json(ArticleId)
        传出：Json(Result,Reason)
    */
    Json::Value DeleteSolution(Json::Value &deletejson);

    // ++++++++++++++++++++++评论表 Comment+++++++++++++++++++++++
    /*
        功能：管理员查询评论
        传入：Json(Page,PageSize)
        传出：Json(_id,ParentId,ParentType,Content,CreateTime,
            Child_Comments._id,Child_Comments.Content,Child_Comments.CreateTime)
    */
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    /*
        功能：查询父评论
        传入：Json(ParentId,Skip,Limie,SonNum)
        传出：
        Json(ParentId,Content,Likes,CreateTime,Child_Total,
        User(Avatar,NickName),
        Child_Comments(_id,Content,Likes,CreateTime,User(Avatar,NickName)))
    */
    Json::Value getFatherComment(Json::Value &queryjson);

    /*
        功能：获取子评论
        传入：Json(ParentId,Skip,Limit)
        传出：Json(Child_Total,Child_Comments(_id,Content,Likes,CreateTime,User(NickName,Avatar)))
    */
    Json::Value getSonComment(Json::Value &queryjson);

    /*
        功能：插入父评论
        传入：Json(ParentId,Content,UserId)
        传出：Json(_id,CreateTime)
    */
    Json::Value InsertFatherComment(Json::Value &insertjson);

    /*
        功能：插入子评论
        传入：Json(ParentId,Content,UserId)
        传出：Json(_id,CreateTime)
    */
    Json::Value InsertSonComment(Json::Value &insertjson);

    /*
        功能：删除某一篇文章（讨论，题解，公告）的所有文章，主要服务于删除文章
        传入：Json(ArticleId)
        传出：bool
    */
    bool DeleteArticleComment(Json::Value &deletejson);

    /*
        功能：删除父评论
        传入：Json(CommentId)
        传出：Json(Result,Reason,DeleteNum)
    */
    Json::Value DeleteFatherComment(Json::Value &deletejson);

    /*
        功能：删除子评论
        传入：Json(CommentId)
        传出：Json(Result,Reason,DeleteNum)
    */
    Json::Value DeleteSonComment(Json::Value &deletejson);

    // ++++++++++++++++++++++++评测表 StatusRecord+++++++++++++++++++++++++
    /*
        功能：插入待测评记录
        传入：Json(ProblemId,UserId,UserNickName,ProblemTitle,Language,Code)
        传出：SubmitId测评的ID
    */
    std::string InsertStatusRecord(Json::Value &insertjson);

    /*
        功能：更新测评记录
        传入：Json(SubmitId,Status,RunTime,RunMemory,Length,ComplierInfo,
        TestInfo[(Status,StandardInput,StandardOutput,PersonalOutput,RunTime,RunMemory)])
        传出：bool
    */
    bool UpdateStatusRecord(Json::Value &updatejson);

    /*
        功能：分页查询测评记录
        传入：Json(SearchInfo,PageSize,Page)
        传出：测评全部信息，详情请见MongoDB集合表
    */
    Json::Value SelectStatusRecordList(Json::Value &queryjson);

    /*
        功能：查询测评记录
        传入：Json(SubmitId)
        传出：全部记录，详情请看MongoDB集合表
    */
    Json::Value SelectStatusRecord(Json::Value &queryjson);

    // +++++++++++++++++ Token ++++++++++++++++++++++++++++
    /*
        功能：用户登录通过Token
        传入：Json(UserId)
        传出：Json(Result,Reason,Info(_id,NickName,Avatar,CommentLikes,Solves,Authority))
    */
    Json::Value LoginUserByToken(Json::Value &loginjson);

    /*
        功能：查询所有用户的权限
        传入：void
        传出：Json(Result,_id,Authority)
    */
    Json::Value SelectUserAuthority();

private:
    /*
        功能：获取某一个集合中最大的ID
    */
    int64_t GetMaxId(std::string name);

    MoDB();

    ~MoDB();

private:
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri{};           // 连接配置
    mongocxx::pool pool{uri};      // 连接池

    std::atomic_int64_t m_problemid;      // 题目ID
    std::atomic_int64_t m_statusrecordid; // 测评ID
    std::atomic_int64_t m_commentid;      // 评论ID
    std::atomic_int64_t m_articleid;      // 文章ID
};

#endif