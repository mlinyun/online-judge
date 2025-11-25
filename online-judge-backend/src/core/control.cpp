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
    return UserService::GetInstance()->SelectUserUpdateInfo(queryjson);
}

// 更新用户信息
Json::Value Control::UpdateUserInfo(Json::Value &updatejson) {
    return UserService::GetInstance()->UpdateUserInfo(updatejson);
}

// 删除用户
Json::Value Control::DeleteUser(Json::Value &deletejson) {
    return UserService::GetInstance()->DeleteUser(deletejson);
}

// 用户排名查询
Json::Value Control::SelectUserRank(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserRank(queryjson);
}

// 分页查询用户列表（管理员权限）
Json::Value Control::SelectUserSetInfo(Json::Value &queryjson) {
    return UserService::GetInstance()->SelectUserSetInfo(queryjson);
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
// 查询题目信息（单条）
Json::Value Control::SelectProblem(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblem(queryjson);
}

// 查询题目信息（管理员权限）
Json::Value Control::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblemInfoByAdmin(queryjson);
}

// 编辑题目：包含插入和更新题目（管理员权限）
Json::Value Control::EditProblem(Json::Value &insertjson) {
    Json::Value resjson;
    string editType = insertjson["EditType"].asString();
    if (editType == "Insert") {
        resjson = ProblemService::GetInstance()->InsertProblem(insertjson);
    } else if (editType == "Update") {
        resjson = ProblemService::GetInstance()->UpdateProblem(insertjson);
    }
    TagService::GetInstance()->InitProblemTags();
    return resjson;
}

// 删除题目（管理员权限）
Json::Value Control::DeleteProblem(Json::Value &deletejson) {
    return ProblemService::GetInstance()->DeleteProblem(deletejson);
}

// 分页获取题目列表
Json::Value Control::SelectProblemList(Json::Value &queryjson) {
    return ProblemService::GetInstance()->SelectProblemList(queryjson);
}

// 分页获取题目列表（管理员权限）
Json::Value Control::SelectProblemListByAdmin(Json::Value &queryjson) {
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
    return AnnouncementService::GetInstance()->InsertAnnouncement(insertjson);
}

// 查询公告详细信息，并将其浏览量加 1
Json::Value Control::SelectAnnouncement(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncement(queryjson);
}

// 更新公告（管理员权限）
Json::Value Control::UpdateAnnouncement(Json::Value &updatejson) {
    return AnnouncementService::GetInstance()->UpdateAnnouncement(updatejson);
}

// 删除公告（管理员权限）
Json::Value Control::DeleteAnnouncement(Json::Value &deletejson) {
    return AnnouncementService::GetInstance()->DeleteAnnouncement(deletejson);
}

// 分页查询公告列表
Json::Value Control::SelectAnnouncementList(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncementList(queryjson);
}

// 分页查询公告列表（管理员权限）
Json::Value Control::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}

// 查询公告的详细信息，主要是编辑时的查询
Json::Value Control::SelectAnnouncementByEdit(Json::Value &queryjson) {
    return AnnouncementService::GetInstance()->SelectAnnouncementByEdit(queryjson);
}
// ------------------------------ 公告模块 End ------------------------------

Control::Control() {
    // 构造函数实现
}

Control::~Control() {
    // 析构函数实现
}