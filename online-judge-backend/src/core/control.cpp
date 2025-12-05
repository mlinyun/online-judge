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
#include "utils/response.h"  // 统一响应工具

using namespace std;

// ------------------------------ 用户模块 Start ------------------------------
/**
 * 功能：用户注册
 * 权限：所有用户均可以注册
 */
Json::Value Control::UserRegister(Json::Value &registerjson) {
    return UserService::GetInstance()->UserRegister(registerjson);
}

/**
 * 功能：用户登录
 * 权限：所有用户均可以登录（被封禁的除外：未实现）
 */
Json::Value Control::UserLogin(Json::Value &loginjson) {
    return UserService::GetInstance()->UserLogin(loginjson);
}

/**
 * 功能：查询用户信息
 * 权限：只允许用户本人查询
 */
Json::Value Control::SelectUserInfo(Json::Value &queryjson) {
    // 检查用户是否已登录（全局请求前处理器中已经验证用户登录状态了）
    // Json::Value login_check = UserService::GetInstance()->CheckLogin(queryjson);
    // if (!login_check.isNull()) {
    //     return login_check;
    // }
    // 如果不是本人，无权查询用户信息
    bool is_author = UserService::GetInstance()->IsAuthor(queryjson);
    if (!is_author) {
        return response::Forbidden();
    }
    return UserService::GetInstance()->SelectUserInfo(queryjson);
}

/**
 * 功能：查询用户信息（在设置页面修改用户时使用）
 * 权限：只允许用户本人查询
 */
Json::Value Control::SelectUserUpdateInfo(Json::Value &queryjson) {
    // 如果不是本人，无权查询用户信息
    bool is_author = UserService::GetInstance()->IsAuthor(queryjson);
    if (!is_author) {
        return response::Forbidden();
    }
    return UserService::GetInstance()->SelectUserUpdateInfo(queryjson);
}

/**
 * 功能：更新用户信息
 * 权限：只允许用户本人或者管理员修改
 */
Json::Value Control::UpdateUserInfo(Json::Value &updatejson) {
    // 如果不是本人或者管理员，无权修改用户信息
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(updatejson);
    if (!is_author) {
        return response::Forbidden();
    }
    return UserService::GetInstance()->UpdateUserInfo(updatejson);
}

/**
 * 功能：删除用户
 * 权限：只允许管理员删除
 */
Json::Value Control::DeleteUser(Json::Value &deletejson) {
    // 如果不是管理员，无权删除用户
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return UserService::GetInstance()->DeleteUser(deletejson);
}

/**
 * 功能：用户排名查询
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectUserRank(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserRank(queryjson);
}

/**
 * 功能：分页查询用户列表
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectUserSetInfo(Json::Value &queryjson) {
    // 如果不是管理员，无权查询用户列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return UserService::GetInstance()->SelectUserSetInfo(queryjson);
}

/**
 * 功能：用户登录通过 Token 鉴权
 * 权限：所有用户均可使用
 */
Json::Value Control::LoginUserByToken(Json::Value &loginjson) {
    return UserService::GetInstance()->LoginUserByToken(loginjson);
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
/**
 * 功能：查询题目信息（单条）
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectProblemInfo(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblemInfo(queryjson);
}

/**
 * 功能：查询题目信息（单条）
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权查询题目信息
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return ProblemService::GetInstance()->SelectProblemInfoByAdmin(queryjson);
}

/**
 * 功能：编辑题目：包含插入和更新题目
 * 权限：只允许管理员编辑
 */
Json::Value Control::EditProblem(Json::Value &editjson) {
    // 如果不是管理员，无权编辑题目
    bool is_administrator = UserService::GetInstance()->IsAdministrator(editjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    Json::Value resjson;
    // 获取编辑类型
    string editType = editjson["EditType"].asString();
    // 获取题目数据
    Json::Value problemjson = editjson["ProblemData"];
    // 通过 Token 获取用户的 UserId
    string token = editjson["Token"].asString();
    string userid = UserService::GetInstance()->GetUserIdByToken(token);
    // 通过 UserId 获取用户的 NickName
    string usernickname = UserService::GetInstance()->GetNickNameByUserId(userid);
    problemjson["UserNickName"] = usernickname;
    // 根据编辑类型进行相应操作
    if (editType == "Insert") {
        // 插入题目（只需要传入题目数据即可）
        resjson = ProblemService::GetInstance()->InsertProblem(problemjson);
    } else if (editType == "Update") {
        // 更新题目（只需要传入题目数据即可）
        resjson = ProblemService::GetInstance()->UpdateProblem(problemjson);
    }
    TagService::GetInstance()->InitProblemTags();
    return resjson;
}

/**
 * 功能：删除题目
 * 权限：只允许管理员删除
 */
Json::Value Control::DeleteProblem(Json::Value &deletejson) {
    // 如果不是管理员，无权删除题目
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return ProblemService::GetInstance()->DeleteProblem(deletejson);
}

/**
 * 功能：分页获取题目列表
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectProblemList(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblemList(queryjson);
}

/**
 * 功能：分页获取题目列表
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectProblemListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取题目列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return ProblemService::GetInstance()->SelectProblemListByAdmin(queryjson);
}
// ------------------------------ 题目模块 End ------------------------------

// ------------------------------ 标签模块 Start ------------------------------
/**
 * 功能：获取题目的所有标签
 * 权限：所有用户均可查询
 */
Json::Value Control::GetTags(Json::Value &queryjson) {
    if (queryjson["TagType"].asString() == "Problem") {
        return TagService::GetInstance()->GetProblemTags();
    }
    // 默认返回空的 Json::Value
    return Json::Value();
}
// ------------------------------ 标签模块 End ------------------------------

// ------------------------------ 公告模块 Start ------------------------------
/**
 * 功能：添加公告
 * 权限：只允许管理员添加
 */
Json::Value Control::InsertAnnouncement(Json::Value &insertjson) {
    // 如果不是管理员，无权添加公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(insertjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return AnnouncementService::GetInstance()->InsertAnnouncement(insertjson);
}

/**
 * 功能：查询公告详细信息，并将其浏览量加 1
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectAnnouncement(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncement(queryjson);
}

/**
 * 功能：查询公告的详细信息，主要是编辑时的查询
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectAnnouncementByEdit(Json::Value &queryjson) {
    // 如果不是管理员，无权查看并编辑公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return AnnouncementService::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

/**
 * 功能：更新公告
 * 权限：只允许管理员修改
 */
Json::Value Control::UpdateAnnouncement(Json::Value &updatejson) {
    // 如果不是管理员，无权更新公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(updatejson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return AnnouncementService::GetInstance()->UpdateAnnouncement(updatejson);
}

/**
 * 功能：删除公告
 * 权限：只允许管理员删除
 */
Json::Value Control::DeleteAnnouncement(Json::Value &deletejson) {
    // 如果不是管理员，无权删除公告
    bool is_administrator = UserService::GetInstance()->IsAdministrator(deletejson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    Json::Value resjson = AnnouncementService::GetInstance()->DeleteAnnouncement(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["success"].asBool()) {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["AnnouncementId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

/**
 * 功能：分页查询公告列表
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectAnnouncementList(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncementList(queryjson);
}

/**
 * 功能：分页查询公告列表
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取公告列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return AnnouncementService::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
/**
 * 功能：添加讨论
 * 权限：只允许普通用户及以上添加
 */
Json::Value Control::InsertDiscuss(Json::Value &insertjson) {
    // 如果不是普通用户或以上，无权添加讨论
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUserOrAbove(insertjson);
    if (!is_ordinary_user) {
        return response::Forbidden();
    }
    return DiscussService::GetInstance()->InsertDiscuss(insertjson);
}

/**
 * 功能：查询讨论的详细内容，并且将其浏览量加 1
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectDiscuss(Json::Value &queryjson) {
    return DiscussService::GetInstance()->SelectDiscuss(queryjson);
}

/**
 * 功能：查询讨论的详细信息，主要是编辑时的查询
 * 权限：只允许讨论作者本人或者管理员查询
 */
Json::Value Control::SelectDiscussByEdit(Json::Value &queryjson) {
    // 1. 先查询讨论作者的 UserId
    int64_t discussId = stoll(queryjson["DiscussId"].asString());
    std::string authorId = DiscussService::GetInstance()->GetDiscussAuthorId(discussId);
    if (authorId.empty()) {
        return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
    }
    // 2. 将作者 UserId 注入到 queryjson 中用于权限校验
    queryjson["UserId"] = authorId;
    // 3. 如果不是用户本人或者管理员，无权查看并编辑讨论
    bool is_author_or_above = UserService::GetInstance()->IsAuthorOrAbove(queryjson);
    if (!is_author_or_above) {
        return response::Forbidden();
    }
    return DiscussService::GetInstance()->SelectDiscussByEdit(queryjson);
}

/**
 * 功能：更新讨论
 * 权限：只允许讨论作者本人或者管理员修改
 */
Json::Value Control::UpdateDiscuss(Json::Value &updatejson) {
    // 1. 先查询讨论作者的 UserId
    int64_t discussId = stoll(updatejson["DiscussId"].asString());
    std::string authorId = DiscussService::GetInstance()->GetDiscussAuthorId(discussId);
    if (authorId.empty()) {
        return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
    }
    // 2. 将作者 UserId 注入到 updatejson 中用于权限校验
    updatejson["UserId"] = authorId;
    // 3. 如果不是讨论作者本人或者管理员，无权修改讨论
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(updatejson);
    if (!is_author) {
        return response::Forbidden();
    }
    return DiscussService::GetInstance()->UpdateDiscuss(updatejson);
}

/**
 * 功能：删除讨论
 * 权限：只允许讨论作者本人或者管理员删除
 */
Json::Value Control::DeleteDiscuss(Json::Value &deletejson) {
    // 1. 先查询讨论作者的 UserId
    int64_t discussId = stoll(deletejson["DiscussId"].asString());
    std::string authorId = DiscussService::GetInstance()->GetDiscussAuthorId(discussId);
    if (authorId.empty()) {
        return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
    }
    // 2. 将作者 UserId 注入到 deletejson 中用于权限校验
    deletejson["UserId"] = authorId;
    // 3. 如果不是讨论作者本人或者管理员，无权删除讨论
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(deletejson);
    if (!is_author) {
        return response::Forbidden();
    }
    Json::Value resjson = DiscussService::GetInstance()->DeleteDiscuss(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["success"].asBool()) {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["DiscussId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

/**
 * 功能：分页查询讨论
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectDiscussList(Json::Value &queryjson) {
    return DiscussService::GetInstance()->SelectDiscussList(queryjson);
}

/**
 * 功能：分页查询讨论
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectDiscussListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取讨论列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return DiscussService::GetInstance()->SelectDiscussListByAdmin(queryjson);
}
// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
/**
 * 功能：添加题解
 * 权限：只允许普通用户及以上添加
 */
Json::Value Control::InsertSolution(Json::Value &insertjson) {
    // 如果不是普通用户，无权添加题解
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUserOrAbove(insertjson);
    if (!is_ordinary_user) {
        return response::Forbidden();
    }
    return SolutionService::GetInstance()->InsertSolution(insertjson);
}

/**
 * 功能：查询题解的详细内容，并且将其浏览量加 1
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectSolution(Json::Value &queryjson) {
    return SolutionService::GetInstance()->SelectSolution(queryjson);
}

/**
 * 功能：查询题解的详细信息，主要是编辑时的查询
 * 权限：只允许题解作者本人或者管理员查询
 */
Json::Value Control::SelectSolutionByEdit(Json::Value &queryjson) {
    // 1. 先查询题解作者的 UserId
    int64_t solutionId = stoll(queryjson["SolutionId"].asString());
    std::string authorId = SolutionService::GetInstance()->GetSolutionAuthorId(solutionId);
    if (authorId.empty()) {
        return response::SolutionNotFound();
    }
    // 2. 将作者 UserId 注入到 queryjson 中用于权限校验
    queryjson["UserId"] = authorId;
    // 3. 如果不是题解作者本人或者管理员，无权查看并编辑题解
    bool is_author_or_above = UserService::GetInstance()->IsAuthorOrAbove(queryjson);
    if (!is_author_or_above) {
        return response::Forbidden();
    }
    return SolutionService::GetInstance()->SelectSolutionByEdit(queryjson);
}

/**
 * 功能：更新题解
 * 权限：只允许题解作者本人或者管理员修改
 */
Json::Value Control::UpdateSolution(Json::Value &updatejson) {
    // 1. 先查询题解作者的 UserId
    int64_t solutionId = stoll(updatejson["SolutionId"].asString());
    std::string authorId = SolutionService::GetInstance()->GetSolutionAuthorId(solutionId);
    if (authorId.empty()) {
        return response::SolutionNotFound();
    }
    // 2. 将作者 UserId 注入到 updatejson 中用于权限校验
    updatejson["UserId"] = authorId;
    // 3. 如果不是题解作者本人或者管理员，无权修改题解
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(updatejson);
    if (!is_author) {
        return response::Forbidden();
    }
    return SolutionService::GetInstance()->UpdateSolution(updatejson);
}

/**
 * 功能：删除题解
 * 权限：只允许题解作者本人或者管理员删除
 */
Json::Value Control::DeleteSolution(Json::Value &deletejson) {
    // 1. 先查询题解作者的 UserId
    int64_t solutionId = stoll(deletejson["SolutionId"].asString());
    std::string authorId = SolutionService::GetInstance()->GetSolutionAuthorId(solutionId);
    if (authorId.empty()) {
        return response::SolutionNotFound();
    }
    // 2. 将作者 UserId 注入到 deletejson 中用于权限校验
    deletejson["UserId"] = authorId;
    // 3. 如果不是题解作者本人或者管理员，无权删除题解
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(deletejson);
    if (!is_author) {
        return response::Forbidden();
    }
    Json::Value resjson = SolutionService::GetInstance()->DeleteSolution(deletejson);
    // 评论模块完成时，添加相应的评论删除操作
    if (resjson["success"].asBool()) {
        Json::Value commentjson;
        commentjson["ArticleId"] = deletejson["SolutionId"];
        CommentService::GetInstance()->DeleteArticleComment(commentjson);
    }
    return resjson;
}

/**
 * 功能：分页查询题解（公开题解）
 * 权限：所有用户均可查询
 */
Json::Value Control::SelectSolutionList(Json::Value &queryjson) {
    return SolutionService::GetInstance()->SelectSolutionList(queryjson);
}

/**
 * 功能：分页查询题解
 * 权限：只允许管理员查询
 */
Json::Value Control::SelectSolutionListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取题解列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return SolutionService::GetInstance()->SelectSolutionListByAdmin(queryjson);
}
// ------------------------------ 题解模块 End ------------------------------

// ------------------------------ 评论模块 Start ------------------------------
/**
 * 功能：插入评论
 * 权限：只允许普通用户及以上添加
 */
Json::Value Control::InsertComment(Json::Value &insertjson) {
    // 如果不是普通用户及以上，无权添加评论
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUserOrAbove(insertjson);
    if (!is_ordinary_user) {
        return response::Forbidden();
    }
    // 根据评论类型调用不同的插入方法
    if (insertjson["CommentType"].asString() == "Father") {
        return CommentService::GetInstance()->InsertFatherComment(insertjson);
    } else if (insertjson["CommentType"].asString() == "Reply" && insertjson["ParentType"].asString() == "Comment") {
        return CommentService::GetInstance()->InsertSonComment(insertjson);
    }
    // 默认返回失败结果
    return response::CommentTypeInvalid();
}

/**
 * 功能：获取评论
 * 权限：所有用户均可查询
 */
Json::Value Control::GetComment(Json::Value &queryjson) {
    if (queryjson["CommentType"].asString() == "Father") {
        return CommentService::GetInstance()->GetFatherComment(queryjson);
    } else if (queryjson["CommentType"].asString() == "Reply") {
        return CommentService::GetInstance()->GetSonComment(queryjson);
    } else {
        // 默认返回失败结果
        return response::CommentTypeInvalid();
    }
}

// 管理员查询评论
Json::Value Control::SelectCommentListByAdmin(Json::Value &queryjson) {
    // 如果不是管理员，无权获取评论列表
    bool is_administrator = UserService::GetInstance()->IsAdministrator(queryjson);
    if (!is_administrator) {
        return response::Forbidden();
    }
    return CommentService::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 删除评论
Json::Value Control::DeleteComment(Json::Value &deletejson) {
    // 1. 先查询评论作者的 UserId
    int64_t commentId = stoll(deletejson["CommentId"].asString());
    std::string authorId = CommentService::GetInstance()->GetCommentAuthorId(commentId);
    if (authorId.empty()) {
        return response::CommentNotFound();
    }
    // 2. 将作者 UserId 注入到 deletejson 中用于权限校验
    deletejson["UserId"] = authorId;
    // 3. 如果不是评论作者本人或者管理员，无权删除评论
    bool is_author = UserService::GetInstance()->IsAuthorOrAbove(deletejson);
    if (!is_author) {
        return response::Forbidden();
    }
    // 先默认用户上传的评论为父评论，尝试删除父评论
    Json::Value json = CommentService::GetInstance()->DeleteFatherComment(deletejson);
    // 如果删除父评论失败，再尝试删除子评论
    if (!json["success"].asBool()) {
        json = CommentService::GetInstance()->DeleteSonComment(deletejson);
    }
    return json;
}
// ------------------------------ 评论模块 End ------------------------------

// ------------------------------ 测评记录模块 Start ------------------------------
/**
 * 功能：查询一条详细测评记录
 * 权限：只允许状态记录作者本人或者管理员查询
 */
Json::Value Control::SelectStatusRecord(Json::Value &queryjson) {
    // 1. 先查询状态记录作者的 UserId
    int64_t statusRecordId = stoll(queryjson["StatusRecordId"].asString());
    std::string authorId = StatusRecordService::GetInstance()->GetStatusRecordAuthorId(statusRecordId);
    if (authorId.empty()) {
        return response::Fail(error_code::STATUS_RECORD_NOT_FOUND, "测评记录不存在！");
    }
    // 2. 将作者 UserId 注入到 queryjson 中用于权限校验
    queryjson["UserId"] = authorId;
    // 3. 如果不是状态记录作者本人或者管理员，无权查询测评记录
    bool is_author_or_above = UserService::GetInstance()->IsAuthorOrAbove(queryjson);
    if (!is_author_or_above) {
        return response::Forbidden();
    }
    // 4. 查询测评记录
    return StatusRecordService::GetInstance()->SelectStatusRecord(queryjson);
}

// 返回状态记录的信息
Json::Value Control::SelectStatusRecordList(Json::Value &queryjson) {
    return StatusRecordService::GetInstance()->SelectStatusRecordList(queryjson);
}
// ------------------------------ 测评记录模块 End ------------------------------

// ------------------------------ 判题模块 Start ------------------------------
// 返回判题信息
Json::Value Control::GetJudgeCode(Json::Value judgejson) {
    // 传入 Json(ProblemId, Code, Language, Token)
    // 如果不是普通用户，无权进行判题
    bool is_ordinary_user = UserService::GetInstance()->IsOrdinaryUserOrAbove(judgejson);
    if (!is_ordinary_user) {
        return response::Forbidden();
    }
    // 通过 Token 获取用户的 UserId
    string token = judgejson["Token"].asString();
    string userid = UserService::GetInstance()->GetUserIdByToken(token);
    judgejson["UserId"] = userid;
    // 通过 UserId 获取用户的 NickName
    string usernickname = UserService::GetInstance()->GetNickNameByUserId(userid);
    judgejson["UserNickName"] = usernickname;
    // 通过 ProblemId 查询题目信息（单条）
    Json::Value queryjson;
    queryjson["ProblemId"] = judgejson["ProblemId"];
    Json::Value problemjson = ProblemService::GetInstance()->SelectProblemInfo(queryjson);
    if (!problemjson["success"].asBool()) {
        return response::ProblemNotFound();
    }
    // 提取题目信息
    judgejson["ProblemTitle"] = problemjson["data"]["Title"];
    judgejson["TimeLimit"] = problemjson["data"]["TimeLimit"];
    judgejson["MemoryLimit"] = problemjson["data"]["MemoryLimit"];
    judgejson["JudgeNum"] = problemjson["data"]["JudgeNum"];

    // 添加状态记录
    // 传入：Json(ProblemId, UserId, UserNickName, ProblemTitle, Language, Code);
    // 构造插入状态记录的 JSON 对象
    Json::Value insertjson;
    insertjson["ProblemId"] = judgejson["ProblemId"];
    insertjson["UserId"] = judgejson["UserId"];
    insertjson["UserNickName"] = judgejson["UserNickName"];
    insertjson["ProblemTitle"] = judgejson["ProblemTitle"];
    insertjson["Language"] = judgejson["Language"];
    insertjson["Code"] = judgejson["Code"];

    // 获取插入的状态记录 ID，即 StatusRecordId
    string status_record_id = StatusRecordService::GetInstance()->InsertStatusRecord(insertjson);
    // 如果插入失败，返回错误信息
    if (status_record_id == "0") {
        return response::Fail(error_code::INTERNAL_ERROR, "系统出错！");
    }

    // 运行代码
    // Json(StatusRecordId, ProblemId, JudgeNum, Code, Language, TimeLimit, MemoryLimit)
    Json::Value runjson;
    runjson["Code"] = judgejson["Code"];
    runjson["StatusRecordId"] = status_record_id;
    runjson["ProblemId"] = judgejson["ProblemId"];
    runjson["Language"] = judgejson["Language"];
    runjson["JudgeNum"] = judgejson["JudgeNum"];
    runjson["TimeLimit"] = judgejson["TimeLimit"];
    runjson["MemoryLimit"] = judgejson["MemoryLimit"];

    // 创建判题对象
    Judger judger;
    Json::Value json = judger.Run(runjson);

    // 判题结束后，需要更新测评记录的状态信息，并且更新题目和用户的状态信息

    /**
     * 更新测评记录
     * 传入：Json(StatusRecordId, Status, RunTime, RunMemory, Length, CompilerInfo,
     * TestInfo[(Status, RunTime, RunMemory, StandardInput, StandardOutput, PersonalOutput)])
     * 传出：bool
     */
    StatusRecordService::GetInstance()->UpdateStatusRecord(json);

    /**
     * 更新题目状态
     * 传入：Json(ProblemId, Status)
     * 传出：bool
     */
    Json::Value updatejson;
    updatejson["ProblemId"] = judgejson["ProblemId"];
    updatejson["Status"] = json["Status"];
    ProblemService::GetInstance()->UpdateProblemStatusNum(updatejson);

    /**
     * 更新用户题目状态
     * 传入：Json(UserId, ProblemId, Status)
     * 传出：bool （是否为该题目的第一次 AC）
     */
    updatejson["UserId"] = judgejson["UserId"];
    bool is_first_ac = UserService::GetInstance()->UpdateUserProblemInfo(updatejson);
    Json::Value data;
    if (is_first_ac) {
        data["IsFirstAC"] = true;
    } else {
        data["IsFirstAC"] = false;
    }
    data["Status"] = json["Status"];
    data["CompilerInfo"] = json["CompilerInfo"];
    data["StatusRecordId"] = status_record_id;
    return response::Success("判题完成", data);
}
// ------------------------------ 判题模块 End ------------------------------

Control::Control() {
    // 构造函数实现
    // 初始化题目标签
    TagService::GetInstance()->InitProblemTags();

    // 初始化用户权限
    UserService::GetInstance()->InitUserAuthority();
}

Control::~Control() {
    // 析构函数实现
}