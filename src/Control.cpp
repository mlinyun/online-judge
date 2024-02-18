#include "Control.h"
#include "ProblemList.h"
#include "UserList.h"
#include "StatusRecordList.h"
#include "DiscussList.h"
#include "CommentList.h"
#include "SolutionList.h"
#include "AnnouncementList.h"
#include "Tag.h"
#include "Judger.h"
#include <iostream>

using namespace std;

// --------------------用户------------------------
Json::Value Control::RegisterUser(Json::Value &registerjson) {
    return UserList::GetInstance()->RegisterUser(registerjson);
}

Json::Value Control::LoginUser(Json::Value &loginjson) {
    return UserList::GetInstance()->LoginUser(loginjson);
}

Json::Value Control::SelectUserRank(Json::Value &queryjson) {
    return UserList::GetInstance()->SelectUserRank(queryjson);
}

Json::Value Control::SelectUserInfo(Json::Value &queryjson) {
    return UserList::GetInstance()->SelectUserInfo(queryjson);
}

Json::Value Control::UpdateUserInfo(Json::Value &updatejson) {
    return UserList::GetInstance()->UpdateUserInfo(updatejson);
}

Json::Value Control::SelectUserUpdateInfo(Json::Value &queryjson) {
    return UserList::GetInstance()->SelectUserUpdateInfo(queryjson);
}

Json::Value Control::SelectUserSetInfo(Json::Value &queryjson) {
    return UserList::GetInstance()->SelectUserSetInfo(queryjson);
}

Json::Value Control::DeleteUser(Json::Value &deletejson) {
    return UserList::GetInstance()->DeleteUser(deletejson);
}

// --------------------题目------------------------
Json::Value Control::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    return ProblemList::GetInstance()->SelectProblemInfoByAdmin(queryjson);
}

Json::Value Control::SelectProblem(Json::Value &queryjson) {
    return ProblemList::GetInstance()->SelectProblem(queryjson);
}

Json::Value Control::EditProblem(Json::Value &insertjson) {
    Json::Value resjson;
    if (insertjson["EditType"].asString() == "Insert") {
        resjson = ProblemList::GetInstance()->InsertProblem(insertjson);
    } else if (insertjson["EditType"].asString() == "Update") {
        resjson = ProblemList::GetInstance()->UpdateProblem(insertjson);
    }
    Tag::GetInstance()->InitProblemTags();
    return resjson;
}

Json::Value Control::DeleteProblem(Json::Value &deletejson) {
    return ProblemList::GetInstance()->DeleteProblem(deletejson);
}

Json::Value Control::SelectProblemList(Json::Value &queryjson) {
    return ProblemList::GetInstance()->SelectProblemList(queryjson);
}

Json::Value Control::SelectProblemListByAdmin(Json::Value &queryjson) {
    return ProblemList::GetInstance()->SelectProblemListByAdmin(queryjson);
}

// --------------------标签------------------------
Json::Value Control::GetTags(Json::Value &queryjson) {
    if (queryjson["TagType"].asString() == "Problem") {
        return Tag::GetInstance()->getProblemTags();
    }
}

// --------------------公告------------------------
Json::Value Control::SelectAnnouncementList(Json::Value &queryjson) {
    return AnnouncementList::GetInstance()->SelectAnnouncementList(queryjson);
}

Json::Value Control::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    return AnnouncementList::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}

Json::Value Control::SelectAnnouncement(Json::Value &queryjson) {
    return AnnouncementList::GetInstance()->SelectAnnouncement(queryjson);
}

Json::Value Control::SelectAnnouncementByEdit(Json::Value &queryjson) {
    return AnnouncementList::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

Json::Value Control::InsertAnnouncement(Json::Value &insertjson) {
    return AnnouncementList::GetInstance()->InsertAnnouncement(insertjson);
}

Json::Value Control::UpdateAnnouncement(Json::Value &updatejson) {
    return AnnouncementList::GetInstance()->UpdateAnnouncement(updatejson);
}

Json::Value Control::DeleteAnnouncement(Json::Value &deletejson) {
    Json::Value resjson = AnnouncementList::GetInstance()->DeleteAnnouncement(deletejson);

    // 当评论模块完成时，将下面注释去掉
    if (resjson["Result"].asString() == "Success") {
        Json::Value json;
        json["ArticleId"] = deletejson["AnnouncementId"];
        CommentList::GetInstance()->DeleteArticleComment(json);
    }
    return resjson;
}

// ----------------------讨论----------------------------
Json::Value Control::SelectDiscussList(Json::Value &queryjson) {
    return DiscussList::GetInstance()->SelectDiscussList(queryjson);
}

Json::Value Control::SelectDiscussListByAdmin(Json::Value &queryjson) {
    return DiscussList::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

Json::Value Control::SelectDiscuss(Json::Value &queryjson) {
    return DiscussList::GetInstance()->SelectDiscuss(queryjson);
}

Json::Value Control::SelectDiscussByEdit(Json::Value &queryjson) {
    return DiscussList::GetInstance()->SelectDiscussByEdit(queryjson);
}

Json::Value Control::InsertDiscuss(Json::Value &insertjson) {
    return DiscussList::GetInstance()->InsertDiscuss(insertjson);
}

Json::Value Control::UpdateDiscuss(Json::Value &updatejson) {
    return DiscussList::GetInstance()->UpdateDiscuss(updatejson);
}

Json::Value Control::DeleteDiscuss(Json::Value &deletejson) {
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

// ----------------------题解----------------------------
Json::Value Control::SelectSolutionList(Json::Value &queryjson) {
    return SolutionList::GetInstance()->SelectSolutionList(queryjson);
}

Json::Value Control::SelectSolutionListByAdmin(Json::Value &queryjson) {
    return SolutionList::GetInstance()->SelectSolutionListByAdmin(queryjson);
}

Json::Value Control::SelectSolution(Json::Value &queryjson) {
    return SolutionList::GetInstance()->SelectSolution(queryjson);
}

Json::Value Control::SelectSolutionByEdit(Json::Value &queryjson) {
    return SolutionList::GetInstance()->SelectSolutionByEdit(queryjson);
}

Json::Value Control::InsertSolution(Json::Value &insertjson) {
    return SolutionList::GetInstance()->InsertSolution(insertjson);
}

Json::Value Control::UpdateSolution(Json::Value &updatejson) {
    return SolutionList::GetInstance()->UpdateSolution(updatejson);
}

Json::Value Control::DeleteSolution(Json::Value &deletejson) {
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

// ----------------------评论----------------------------
Json::Value Control::SelectCommentListByAdmin(Json::Value &queryjson) {
    return CommentList::GetInstance()->SelectCommentListByAdmin(queryjson);
}

Json::Value Control::GetComment(Json::Value &queryjson) {
    if (queryjson["Type"].asString() == "Father") {
        return CommentList::GetInstance()->getFatherComment(queryjson);
    } else {
        return CommentList::GetInstance()->getSonComment(queryjson);
    }
}

Json::Value Control::InsertComment(Json::Value &insertjson) {
    // 文章添加评论数
    Json::Value updatejson;
    updatejson["ArticleId"] = insertjson["ArticleId"];
    updatejson["Num"] = 1;
    if (insertjson["ArticleType"].asString() == "Discuss") {
        DiscussList::GetInstance()->UpdateDiscussComments(updatejson);
    } else if (insertjson["ArticleType"].asString() == "Solution") {
        SolutionList::GetInstance()->UpdateSolutionComments(updatejson);
    } else if (insertjson["ArticleType"].asString() == "Announcement") {
        AnnouncementList::GetInstance()->UpdateAnnouncementComments(updatejson);
    }

    if (insertjson["Type"].asString() == "Father") // 父评论
    {

        return CommentList::GetInstance()->InsertFatherComment(insertjson);
    } else // 子评论
    {
        return CommentList::GetInstance()->InsertSonComment(insertjson);
    }
}

// Json(ArticleId,CommentId)
Json::Value Control::DeleteComment(Json::Value &deletejson) {
    string articleid = deletejson["ArticleId"].asString();

    Json::Value resjson;
    // 删除父评论
    Json::Value json = CommentList::GetInstance()->DeleteFatherComment(deletejson);
    // 如果失败删除子评论
    if (json["Result"] == "Fail")
        json = CommentList::GetInstance()->DeleteSonComment(deletejson);
    // 如果都失败，返回失败结果
    if (json["Result"] == "Fail") {
        resjson["Result"] = "Fail";
        resjson["Reason"] = "数据库未查询到数据！";
        return resjson;
    }
    // 如果删除评论成功，更新文章的评论数量
    Json::Value articlejson;
    articlejson["Num"] = stoi(json["DeleteNum"].asString()) * -1;
    articlejson["ArticleId"] = articleid;
    string articletype = json["ArticleType"].asString();
    if (articletype == "Discuss") {
        DiscussList::GetInstance()->UpdateDiscussComments(articlejson);
    } else if (articletype == "Solution") {
        SolutionList::GetInstance()->UpdateSolutionComments(articlejson);
    } else if (articletype == "Announcement") {
        AnnouncementList::GetInstance()->UpdateAnnouncementComments(articlejson);
    }

    resjson["Result"] = "Success";
    return resjson;
}

// 构造函数
Control::Control() {
    // 初始化题目标签
    Tag::GetInstance()->InitProblemTags();
}

// 析构函数
Control::~Control() {
}