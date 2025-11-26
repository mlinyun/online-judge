#include "core/control.h"

#include <iostream>

#include "judger/judger.h"
#include "services/announcement_service.h"
#include "services/comment_service.h"
#include "services/discuss_service.h"
#include "services/problem_service.h"
#include "services/solution_service.h"
#include "services/status_record_service.h"
#include "services/tag_service.h"
#include "services/user_service.h"

using namespace std;

// 无权限（该变量用于当无权限情况时，直接返回该变量）
static Json::Value NoPermission;

// ------------------------------ 用户模块 Start ------------------------------
// 用户注册
Json::Value Control::RegisterUser(Json::Value &registerjson) {
    return UserService::GetInstance()->RegisterUser(registerjson);
}

// 用户登录
Json::Value Control::LoginUser(Json::Value &loginjson) {
    return UserService::GetInstance()->LoginUser(loginjson);
}

// 查询用户信息
Json::Value Control::SelectUserInfo(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserInfo(queryjson);
}

// 查询用户信息（在设置页面修改用户时使用）
Json::Value Control::SelectUserUpdateInfo(Json::Value &queryjson) {
    // 如果不是本人或者管理员，无权修改用户信息
    bool is_author = UserService::GetInstance()->IsAuthor(queryjson);
    if (!is_author) {
        return NoPermission;
    }
    return UserService::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 更新用户信息
Json::Value Control::UpdateUserInfo(Json::Value &updatejson) {
    // 如果不是本人或者管理员，无权修改用户信息
    bool is_author = UserService::GetInstance()->IsAuthor(updatejson);
    if (!is_author) {
        return NoPermission;
    }
    return UserService::GetInstance()->UpdateUserInfo(updatejson);
}

// 删除用户
Json::Value Control::DeleteUser(Json::Value &deletejson) {
    // 如果不是管理员，无权删除用户
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return NoPermission;
    }
    return UserService::GetInstance()->DeleteUser(deletejson);
}

// 用户排名查询
Json::Value Control::SelectUserRank(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserRank(queryjson);
}

// 分页查询用户列表（管理员权限）
Json::Value Control::SelectUserSetInfo(Json::Value &queryjson) {
    // 如果不是管理员，无权查询用户列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return UserService::GetInstance()->SelectUserSetInfo(queryjson);
}

// 用户登录通过 Token 鉴权（Token 鉴权实现）
Json::Value Control::LoginUserByToken(Json::Value &loginjson) {
    return UserService::GetInstance()->LoginUserByToken(loginjson);
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
// 查询题目信息（单条）
Json::Value Control::SelectProblem(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblem(queryjson);
}

// 查询题目信息（管理员权限）
Json::Value Control::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权查询题目信息
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return ProblemService::GetInstance()->SelectProblemInfoByAdmin(queryjson);
}

// 编辑题目：包含插入和更新题目（管理员权限）
Json::Value Control::EditProblem(Json::Value &insertjson) {
    // 如果不是管理员，无权编辑题目
    bool is_administrator = UserService::GetInstance()->IsAdministrator(insertjson);
    if (!is_administrator) {
        return NoPermission;
    }
    Json::Value resjson;
    // 获取编辑类型
    string editType = insertjson["EditType"].asString();
    // 根据编辑类型进行相应操作
    if (editType == "Insert") {
        // 插入题目
        resjson = ProblemService::GetInstance()->InsertProblem(insertjson);
    } else if (editType == "Update") {
        // 更新题目
        resjson = ProblemService::GetInstance()->UpdateProblem(insertjson);
    }
    TagService::GetInstance()->InitProblemTags();
    return resjson;
}

// 删除题目（管理员权限）
Json::Value Control::DeleteProblem(Json::Value &deletejson) {
    // 如果不是管理员，无权删除题目
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return NoPermission;
    }
    return ProblemService::GetInstance()->DeleteProblem(deletejson);
}

// 分页获取题目列表
Json::Value Control::SelectProblemList(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblemList(queryjson);
}

// 分页获取题目列表（管理员权限）
Json::Value Control::SelectProblemListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取题目列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return ProblemService::GetInstance()->SelectProblemListByAdmin(queryjson);
}
// ------------------------------ 题目模块 End ------------------------------

// ------------------------------ 标签模块 Start ------------------------------
// 获取题目的所有标签
Json::Value Control::GetTags(Json::Value &queryjson) {
    if (queryjson["TagType"].asString() == "Problem") {
        return TagService::GetInstance()->GetProblemTags();
    }
    // 默认返回空的 Json::Value
    return Json::Value();
}
// ------------------------------ 标签模块 End ------------------------------

// ------------------------------ 公告模块 Start ------------------------------
// 添加公告（管理员权限）
Json::Value Control::InsertAnnouncement(Json::Value &insertjson) {
    // 如果不是管理员，无权添加公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(insertjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return AnnouncementService::GetInstance()->InsertAnnouncement(insertjson);
}

// 查询公告详细信息，并将其浏览量加 1
Json::Value Control::SelectAnnouncement(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncement(queryjson);
}

// 更新公告（管理员权限）
Json::Value Control::UpdateAnnouncement(Json::Value &updatejson) {
    // 如果不是管理员，无权更新公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(updatejson);
    if (!is_administrator) {
        return NoPermission;
    }
    return AnnouncementService::GetInstance()->UpdateAnnouncement(updatejson);
}

// 删除公告（管理员权限）
Json::Value Control::DeleteAnnouncement(Json::Value &deletejson) {
    // 如果不是管理员，无权删除公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return NoPermission;
    }
    Json::Value resjson = AnnouncementService::GetInstance()->DeleteAnnouncement(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["Result"].asString() == "Success") {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["AnnouncementId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

// 分页查询公告列表
Json::Value Control::SelectAnnouncementList(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncementList(queryjson);
}

// 分页查询公告列表（管理员权限）
Json::Value Control::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取公告列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return AnnouncementService::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}

// 查询公告的详细信息，主要是编辑时的查询（管理员权限）
Json::Value Control::SelectAnnouncementByEdit(Json::Value &queryjson) {
    // 如果不是管理员，无权查看并编辑公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return AnnouncementService::GetInstance()->SelectAnnouncementByEdit(queryjson);
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
// 添加讨论
Json::Value Control::InsertDiscuss(Json::Value &insertjson) {
    // 如果不是普通用户，无权添加讨论
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(insertjson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    return DiscussService::GetInstance()->InsertDiscuss(insertjson);
}

// 查询讨论的详细内容，并且将其浏览量加 1
Json::Value Control::SelectDiscuss(Json::Value &queryjson) {
    return DiscussService::GetInstance()->SelectDiscuss(queryjson);
}

// 更新讨论
Json::Value Control::UpdateDiscuss(Json::Value &updatejson) {
    // 如果不是讨论作者本人或者管理员，无权修改讨论
    bool is_author = UserService::GetInstance()->IsAuthor(updatejson);
    if (!is_author) {
        return NoPermission;
    }
    return DiscussService::GetInstance()->UpdateDiscuss(updatejson);
}

// 删除讨论
Json::Value Control::DeleteDiscuss(Json::Value &deletejson) {
    // 如果不是讨论作者本人或者管理员，无权删除讨论
    bool is_author = UserService::GetInstance()->IsAuthor(deletejson);
    if (!is_author) {
        return NoPermission;
    }
    Json::Value resjson = DiscussService::GetInstance()->DeleteDiscuss(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["Result"].asString() == "Success") {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["DiscussId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

// 分页查询讨论
Json::Value Control::SelectDiscussList(Json::Value &queryjson) {
    return DiscussService::GetInstance()->SelectDiscussList(queryjson);
}

// 分页查询讨论（管理员权限）
Json::Value Control::SelectDiscussListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取讨论列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return DiscussService::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

// 查询讨论的详细信息，主要是编辑时的查询
Json::Value Control::SelectDiscussByEdit(Json::Value &queryjson) {
    // 如果不是用户本人，无权查看并编辑讨论
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(queryjson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    return DiscussService::GetInstance()->SelectDiscussByEdit(queryjson);
}
// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
// 添加题解
Json::Value Control::InsertSolution(Json::Value &insertjson) {
    // 如果不是普通用户，无权添加题解
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(insertjson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    return SolutionService::GetInstance()->InsertSolution(insertjson);
}

// 查询题解的详细内容，并且将其浏览量加 1
Json::Value Control::SelectSolution(Json::Value &queryjson) {
    return SolutionService::GetInstance()->SelectSolution(queryjson);
}

// 更新题解
Json::Value Control::UpdateSolution(Json::Value &updatejson) {
    // 如果不是题解作者本人或者管理员，无权修改题解
    bool is_author = UserService::GetInstance()->IsAuthor(updatejson);
    if (!is_author) {
        return NoPermission;
    }
    return SolutionService::GetInstance()->UpdateSolution(updatejson);
}

// 删除题解
Json::Value Control::DeleteSolution(Json::Value &deletejson) {
    // 如果不是题解作者本人或者管理员，无权删除题解
    bool is_author = UserService::GetInstance()->IsAuthor(deletejson);
    if (!is_author) {
        return NoPermission;
    }
    Json::Value resjson = SolutionService::GetInstance()->DeleteSolution(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["Result"].asString() == "Success") {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["SolutionId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

// 分页查询题解（公开题解）
Json::Value Control::SelectSolutionList(Json::Value &queryjson) {
    return SolutionService::GetInstance()->SelectSolutionList(queryjson);
}

// 分页查询题解（管理员权限）
Json::Value Control::SelectSolutionListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取题解列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return SolutionService::GetInstance()->SelectSolutionListByAdmin(queryjson);
}

// 查询题解的详细信息，主要是编辑时的查询
Json::Value Control::SelectSolutionByEdit(Json::Value &queryjson) {
    // 如果不是用户本人，无权查看并编辑题解
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(queryjson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    return SolutionService::GetInstance()->SelectSolutionByEdit(queryjson);
}
// ------------------------------ 题解模块 End ------------------------------

// ------------------------------ 评论模块 Start ------------------------------
// 管理员查询评论
Json::Value Control::SelectCommentListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取评论列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return NoPermission;
    }
    return CommentService::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 获取评论 根据Id
Json::Value Control::GetComment(Json::Value &queryjson) {
    if (queryjson["Type"].asString() == "Father") {
        return CommentService::GetInstance()->getFatherComment(queryjson);
    } else {
        return CommentService::GetInstance()->getSonComment(queryjson);
    }
}

// 插入评论
Json::Value Control::InsertComment(Json::Value &insertjson) {
    // 如果不是普通用户，无权添加评论
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(insertjson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    // 文章添加评论数
    Json::Value updatejson;
    updatejson["ArticleId"] = insertjson["ArticleId"];
    updatejson["Num"] = 1;
    if (insertjson["ArticleType"].asString() == "Discuss") {
        DiscussService::GetInstance()->UpdateDiscussComments(updatejson);
    } else if (insertjson["ArticleType"].asString() == "Solution") {
        SolutionService::GetInstance()->UpdateSolutionComments(updatejson);
    } else if (insertjson["ArticleType"].asString() == "Announcement") {
        AnnouncementService::GetInstance()->UpdateAnnouncementComments(updatejson);
    }

    // 父评论
    if (insertjson["Type"].asString() == "Father") {
        return CommentService::GetInstance()->InsertFatherComment(insertjson);
    } else  // 子评论
    {
        return CommentService::GetInstance()->InsertSonComment(insertjson);
    }
}

// 删除评论
Json::Value Control::DeleteComment(Json::Value &deletejson) {
    // 如果不是评论作者本人或者管理员，无权删除评论
    bool is_author = UserService::GetInstance()->IsAuthor(deletejson);
    if (!is_author) {
        return NoPermission;
    }
    string articleid = deletejson["ArticleId"].asString();

    Json::Value resjson;
    // 删除父评论
    Json::Value json = CommentService::GetInstance()->DeleteFatherComment(deletejson);
    // 如果失败删除子评论
    if (json["Result"] == "Fail")
        json = CommentService::GetInstance()->DeleteSonComment(deletejson);
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
        DiscussService::GetInstance()->UpdateDiscussComments(articlejson);
    } else if (articletype == "Solution") {
        SolutionService::GetInstance()->UpdateSolutionComments(articlejson);
    } else if (articletype == "Announcement") {
        AnnouncementService::GetInstance()->UpdateAnnouncementComments(articlejson);
    }

    resjson["Result"] = "Success";
    return resjson;
}
// ------------------------------ 评论模块 End ------------------------------

// ------------------------------ 测评记录模块 Start ------------------------------
// 返回状态记录的信息
Json::Value Control::SelectStatusRecordList(Json::Value &queryjson) {
    return StatusRecordService::GetInstance()->SelectStatusRecordList(queryjson);
}

// 查询一条详细测评记录
Json::Value Control::SelectStatusRecord(Json::Value &queryjson) {
    return StatusRecordService::GetInstance()->SelectStatusRecord(queryjson);
}
// ------------------------------ 测评记录模块 End ------------------------------

// ------------------------------ 判题模块 Start ------------------------------
// 返回判题信息
Json::Value Control::GetJudgeCode(Json::Value judgejson) {
    // 如果不是普通用户，无权进行判题
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUser(judgejson);
    if (!is_ordinary_user) {
        return NoPermission;
    }
    Json::Value resjson;
    // 传入 Json(ProblemId, UserId, UserNickName, Code, Language, TimeLimit, MemoryLimit, JudgeNum, ProblemTitle)

    // 添加状态记录
    // 传入：Json(ProblemId, UserId, UserNickName, ProblemTitle, Language, Code);
    Json::Value insertjson;
    insertjson["ProblemId"] = judgejson["ProblemId"];
    insertjson["UserId"] = judgejson["UserId"];
    insertjson["UserNickName"] = judgejson["UserNickName"];
    insertjson["ProblemTitle"] = judgejson["ProblemTitle"];
    insertjson["Language"] = judgejson["Language"];
    insertjson["Code"] = judgejson["Code"];

    string submitid = StatusRecordService::GetInstance()->InsertStatusRecord(insertjson);

    if (submitid == "0") {
        resjson["Result"] = "Fail";
        resjson["Reason"] = "系统出错！";
        return resjson;
    }

    // 运行代码
    // Json(SubmitId,ProblemId,JudgeNum,Code,Language,TimeLimit,MemoryLimit)
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
    StatusRecordService::GetInstance()->UpdateStatusRecord(json);
    // 更新题目的状态
    Json::Value updatejson;
    updatejson["ProblemId"] = judgejson["ProblemId"];
    updatejson["Status"] = json["Status"];
    ProblemService::GetInstance()->UpdateProblemStatusNum(updatejson);

    updatejson["UserId"] = judgejson["UserId"];

    // 更新用户的状态
    if (UserService::GetInstance()->UpdateUserProblemInfo(updatejson))
        resjson["IsFirstAC"] = true;
    else
        resjson["IsFirstAC"] = false;

    resjson["Result"] = "Success";
    resjson["Status"] = json["Status"];
    resjson["ComplierInfo"] = json["ComplierInfo"];
    return resjson;
}
// ------------------------------ 判题模块 End ------------------------------

Control::Control() {
    // 构造函数实现
    // 初始化题目标签
    TagService::GetInstance()->InitProblemTags();

    // 初始化用户权限
    UserService::GetInstance()->InitUserAuthority();

    // 初始化无权限返回变量
    NoPermission["Result"] = "401";
    NoPermission["Reason"] = "无权限访问！";
}

Control::~Control() {
    // 析构函数实现
}