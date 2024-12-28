#include "Control.h"          // 控制
#include "UserList.h"         // 用户
#include "ProblemList.h"      // 题目
#include "AnnouncementList.h" // 公告
#include "DiscussList.h"      // 讨论
#include "SolutionList.h"     // 题解
#include "CommentList.h"      // 评论
#include "StatusRecordList.h" // 状态
#include "Tag.h"              // 标签
#include "Judger.h"           // 判题机
#include <iostream>

using namespace std;

// ++++++++++++++++++++++++++++++ 用户模块 Start ++++++++++++++++++++++++++++++
// 注册用户
Json::Value Control::RegisterUser(Json::Value &registerjson)
{
    return UserList::GetInstance()->RegisterUser(registerjson);
}

// 登录用户
Json::Value Control::LoginUser(Json::Value &loginjson)
{
    return UserList::GetInstance()->LoginUser(loginjson);
}

// 获取用户大部分信息
Json::Value Control::SelectUserInfo(Json::Value &queryjson)
{
    return UserList::GetInstance()->SelectUserInfo(queryjson);
}

// 更新用户信息
Json::Value Control::UpdateUserInfo(Json::Value &updatejson)
{
    return UserList::GetInstance()->UpdateUserInfo(updatejson);
}

// 获取用户信息以提供修改
Json::Value Control::SelectUserUpdateInfo(Json::Value &queryjson)
{
    return UserList::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 管理员分页获取用户信息
Json::Value Control::SelectUserSetInfo(Json::Value &queryjson)
{
    return UserList::GetInstance()->SelectUserSetInfo(queryjson);
}

// 删除用户
Json::Value Control::DeleteUser(Json::Value &deletejson)
{
    return UserList::GetInstance()->DeleteUser(deletejson);
}

// 获取用户 Rank 排名
Json::Value Control::SelectUserRank(Json::Value &queryjson)
{
    return UserList::GetInstance()->SelectUserRank(queryjson);
}
// ++++++++++++++++++++++++++++++ 用户模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 题目模块 Start ++++++++++++++++++++++++++++++
// 管理员查看题目详细信息
Json::Value Control::SelectProblemInfoByAdmin(Json::Value &queryjson)
{
    return ProblemList::GetInstance()->SelectProblemInfoByAdmin(queryjson);
}

// 用户查询题目详细信息
Json::Value Control::SelectProblem(Json::Value &queryjson)
{
    return ProblemList::GetInstance()->SelectProblem(queryjson);
}

// 编辑题目
Json::Value Control::EditProblem(Json::Value &insertjson)
{
    Json::Value resjson;
    if (insertjson["EditType"].asString() == "Insert")
    {
        resjson = ProblemList::GetInstance()->InsertProblem(insertjson);
    }
    else if (insertjson["EditType"].asString() == "Update")
    {
        resjson = ProblemList::GetInstance()->UpdateProblem(insertjson);
    }
    Tag::GetInstance()->InitProblemTags();
    return resjson;
}

// 删除题目
Json::Value Control::DeleteProblem(Json::Value &deletejson)
{
    return ProblemList::GetInstance()->DeleteProblem(deletejson);
}

// 通过普通查询获取题库数据
Json::Value Control::SelectProblemList(Json::Value &queryjson)
{
    return ProblemList::GetInstance()->SelectProblemList(queryjson);
}

// 管理员查询列表
Json::Value Control::SelectProblemListByAdmin(Json::Value &queryjson)
{
    return ProblemList::GetInstance()->SelectProblemListByAdmin(queryjson);
}

// 获取标签
Json::Value Control::GetTags(Json::Value &queryjson)
{
    if (queryjson["TagType"].asString() == "Problem")
    {
        return Tag::GetInstance()->getProblemTags();
    }

    // 默认返回空的 Json::Value
    return Json::Value();
}
// ++++++++++++++++++++++++++++++ 题目模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 公告模块 Start ++++++++++++++++++++++++++++++
// 添加公告
Json::Value Control::InsertAnnouncement(Json::Value &insertjson)
{
    return AnnouncementList::GetInstance()->InsertAnnouncement(insertjson);
}

// 分页查询公告
Json::Value Control::SelectAnnouncementList(Json::Value &queryjson)
{
    return AnnouncementList::GetInstance()->SelectAnnouncementList(queryjson);
}

// 管理员分页查询公告
Json::Value Control::SelectAnnouncementListByAdmin(Json::Value &queryjson)
{
    return AnnouncementList::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}

// 查询公告的详细信息
Json::Value Control::SelectAnnouncement(Json::Value &queryjson)
{
    return AnnouncementList::GetInstance()->SelectAnnouncement(queryjson);
}

// 查询公告 进行编辑
Json::Value Control::SelectAnnouncementByEdit(Json::Value &queryjson)
{
    return AnnouncementList::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

// 更新公告
Json::Value Control::UpdateAnnouncement(Json::Value &updatejson)
{
    return AnnouncementList::GetInstance()->UpdateAnnouncement(updatejson);
}

// 删除公告
Json::Value Control::DeleteAnnouncement(Json::Value &deletejson)
{
    Json::Value resjson = AnnouncementList::GetInstance()->DeleteAnnouncement(deletejson);

    // 当评论模块完成时，将下面注释去掉
    if (resjson["Result"].asString() == "Success")
    {
        Json::Value json;
        json["ArticleId"] = deletejson["AnnouncementId"];
        CommentList::GetInstance()->DeleteArticleComment(json);
    }
    return resjson;
}
// ++++++++++++++++++++++++++++++ 公告模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 讨论模块 Start ++++++++++++++++++++++++++++++
// 添加讨论
Json::Value Control::InsertDiscuss(Json::Value &insertjson)
{
    return DiscussList::GetInstance()->InsertDiscuss(insertjson);
}

// 分页查询讨论
Json::Value Control::SelectDiscussList(Json::Value &queryjson)
{
    return DiscussList::GetInstance()->SelectDiscussList(queryjson);
}

// 管理员分页查询讨论
Json::Value Control::SelectDiscussListByAdmin(Json::Value &queryjson)
{
    return DiscussList::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

// 查询讨论的详细内容，并且将其浏览量加一
Json::Value Control::SelectDiscuss(Json::Value &queryjson)
{
    return DiscussList::GetInstance()->SelectDiscuss(queryjson);
}

// 查询讨论的详细信息，主要是编辑时的查询
Json::Value Control::SelectDiscussByEdit(Json::Value &queryjson)
{
    return DiscussList::GetInstance()->SelectDiscussByEdit(queryjson);
}

// 修改评论数的数量
Json::Value Control::UpdateDiscuss(Json::Value &updatejson)
{
    return DiscussList::GetInstance()->UpdateDiscuss(updatejson);
}

// 删除讨论
Json::Value Control::DeleteDiscuss(Json::Value &deletejson)
{
    Json::Value resjson = DiscussList::GetInstance()->DeleteDiscuss(deletejson);

    // 当评论模块完成时，将下面注释去掉
    if (resjson["Result"].asString() == "Success")
    {
        Json::Value json;
        json["ArticleId"] = deletejson["DiscussId"];
        CommentList::GetInstance()->DeleteArticleComment(json);
    }
    return resjson;
}
// ++++++++++++++++++++++++++++++ 讨论模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 题解模块 Start ++++++++++++++++++++++++++++++
// 查询题解列表
Json::Value Control::SelectSolutionList(Json::Value &queryjson)
{
    return SolutionList::GetInstance()->SelectSolutionList(queryjson);
}

// 管理员查询
Json::Value Control::SelectSolutionListByAdmin(Json::Value &queryjson)
{
    return SolutionList::GetInstance()->SelectSolutionListByAdmin(queryjson);
}

// 查询题解
Json::Value Control::SelectSolution(Json::Value &queryjson)
{
    return SolutionList::GetInstance()->SelectSolution(queryjson);
}

// 查询题解进行编辑
Json::Value Control::SelectSolutionByEdit(Json::Value &queryjson)
{
    return SolutionList::GetInstance()->SelectSolutionByEdit(queryjson);
}

// 插入题解
Json::Value Control::InsertSolution(Json::Value &insertjson)
{
    return SolutionList::GetInstance()->InsertSolution(insertjson);
}

// 更新题解
Json::Value Control::UpdateSolution(Json::Value &updatejson)
{
    return SolutionList::GetInstance()->UpdateSolution(updatejson);
}

// 删除题解
Json::Value Control::DeleteSolution(Json::Value &deletejson)
{
    Json::Value resjson = SolutionList::GetInstance()->DeleteSolution(deletejson);

    // 当评论模块完成时，将下面注释去掉
    if (resjson["Result"].asString() == "Success")
    {
        Json::Value json;
        json["ArticleId"] = deletejson["SolutionId"];
        CommentList::GetInstance()->DeleteArticleComment(json);
    }
    return resjson;
}
// ++++++++++++++++++++++++++++++ 题解模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 评论模块 Start ++++++++++++++++++++++++++++++
// 管理员查询评论
Json::Value Control::SelectCommentListByAdmin(Json::Value &queryjson)
{
    return CommentList::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 获取评论 根据 Id
Json::Value Control::GetComment(Json::Value &queryjson)
{
    if (queryjson["Type"].asString() == "Father")
    {
        return CommentList::GetInstance()->getFatherComment(queryjson);
    }
    else
    {
        return CommentList::GetInstance()->getSonComment(queryjson);
    }
}

// 插入评论
Json::Value Control::InsertComment(Json::Value &insertjson)
{
    // 文章添加评论数
    Json::Value updatejson;
    updatejson["ArticleId"] = insertjson["ArticleId"];
    updatejson["Num"] = 1;
    if (insertjson["ArticleType"].asString() == "Discuss")
    {
        DiscussList::GetInstance()->UpdateDiscussComments(updatejson);
    }
    else if (insertjson["ArticleType"].asString() == "Solution")
    {
        SolutionList::GetInstance()->UpdateSolutionComments(updatejson);
    }
    else if (insertjson["ArticleType"].asString() == "Announcement")
    {
        AnnouncementList::GetInstance()->UpdateAnnouncementComments(updatejson);
    }

    if (insertjson["Type"].asString() == "Father") // 父评论
    {

        return CommentList::GetInstance()->InsertFatherComment(insertjson);
    }
    else // 子评论
    {
        return CommentList::GetInstance()->InsertSonComment(insertjson);
    }
}

// 删除评论
Json::Value Control::DeleteComment(Json::Value &deletejson)
{
    string articleid = deletejson["ArticleId"].asString();

    Json::Value resjson;
    // 删除父评论
    Json::Value json = CommentList::GetInstance()->DeleteFatherComment(deletejson);
    // 如果失败删除子评论
    if (json["Result"] == "Fail")
        json = CommentList::GetInstance()->DeleteSonComment(deletejson);
    // 如果都失败，返回失败结果
    if (json["Result"] == "Fail")
    {
        resjson["Result"] = "Fail";
        resjson["Reason"] = "数据库未查询到数据！";
        return resjson;
    }
    // 如果删除评论成功，更新文章的评论数量
    Json::Value articlejson;
    articlejson["Num"] = stoi(json["DeleteNum"].asString()) * -1;
    articlejson["ArticleId"] = articleid;
    string articletype = json["ArticleType"].asString();
    if (articletype == "Discuss")
    {
        DiscussList::GetInstance()->UpdateDiscussComments(articlejson);
    }
    else if (articletype == "Solution")
    {
        SolutionList::GetInstance()->UpdateSolutionComments(articlejson);
    }
    else if (articletype == "Announcement")
    {
        AnnouncementList::GetInstance()->UpdateAnnouncementComments(articlejson);
    }

    resjson["Result"] = "Success";
    return resjson;
}
// ++++++++++++++++++++++++++++++ 评论模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 测评记录模块 Start ++++++++++++++++++++++++++++++
// 返回测评记录列表
Json::Value Control::SelectStatusRecordList(Json::Value &queryjson)
{
    return StatusRecordList::GetInstance()->SelectStatusRecordList(queryjson);
}

// 查询一条详细测评记录
Json::Value Control::SelectStatusRecord(Json::Value &queryjson)
{
    return StatusRecordList::GetInstance()->SelectStatusRecord(queryjson);
}
// ++++++++++++++++++++++++++++++ 测评记录模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 判题模块 Start ++++++++++++++++++++++++++++++
Json::Value Control::GetJudgeCode(Json::Value judgejson)
{
    Json::Value resjson;
    // 传入Json(ProblemId, UserId, UserNickName, Code, Language, TimeLimit, MemoryLimit, JudgeNum, ProblemTitle)

    // 添加状态记录
    // 传入：Json(ProblemId, UserId, UserNickName, ProblemTitle, Language, Code);
    Json::Value insertjson;
    insertjson["ProblemId"] = judgejson["ProblemId"];
    insertjson["UserId"] = judgejson["UserId"];
    insertjson["UserNickName"] = judgejson["UserNickName"];
    insertjson["ProblemTitle"] = judgejson["ProblemTitle"];
    insertjson["Language"] = judgejson["Language"];
    insertjson["Code"] = judgejson["Code"];

    string submitid = StatusRecordList::GetInstance()->InsertStatusRecord(insertjson);

    if (submitid == "0")
    {
        resjson["Result"] = "Fail";
        resjson["Reason"] = "系统出错！";
        return resjson;
    }

    // 运行代码
    // Json(SubmitId, ProblemId, JudgeNum, Code, Language, TimeLimit, MemoryLimit)
    Json::Value runjson;
    runjson["Code"] = judgejson["Code"];
    runjson["SubmitId"] = submitid;
    runjson["ProblemId"] = judgejson["ProblemId"];
    runjson["Language"] = judgejson["Language"];
    runjson["JudgeNum"] = judgejson["JudgeNum"];
    runjson["TimeLimit"] = judgejson["TimeLimit"];
    runjson["MemoryLimit"] = judgejson["MemoryLimit"];

    // 创建判题对象
    Judger judger;
    Json::Value json = judger.Run(runjson);

    // 更新状态信息
    /*
        传入：Json(SubmitId,Status,RunTime,RunMemory,Length,ComplierInfo,
        TestInfo[(Status,StandardOutput,PersonalOutput,RunTime,RunMemory)])
    */
    StatusRecordList::GetInstance()->UpdateStatusRecord(json);
    // 更新题目的状态
    Json::Value updatejson;
    updatejson["ProblemId"] = judgejson["ProblemId"];
    updatejson["Status"] = json["Status"];
    ProblemList::GetInstance()->UpdateProblemStatusNum(updatejson);

    updatejson["UserId"] = judgejson["UserId"];

    // 更新用户的状态
    if (UserList::GetInstance()->UpdateUserProblemInfo(updatejson))
        resjson["IsFirstAC"] = true;
    else
        resjson["IsFirstAC"] = false;

    resjson["Result"] = "Success";
    resjson["Status"] = json["Status"];
    resjson["ComplierInfo"] = json["ComplierInfo"];
    return resjson;
}
// ++++++++++++++++++++++++++++++ 判题模块 End ++++++++++++++++++++++++++++++

Control::Control()
{
    // 初始化题目标签
    Tag::GetInstance()->InitProblemTags();
}

Control::~Control()
{
}