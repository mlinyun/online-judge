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

// 用户
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

// 题目
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

Json::Value Control::GetTags(Json::Value &queryjson) {
    if (queryjson["TagType"].asString() == "Problem") {
        return Tag::GetInstance()->getProblemTags();
    }
}

// 构造函数
Control::Control() {
    // 初始化题目标签
    Tag::GetInstance()->InitProblemTags();
}

// 析构函数
Control::~Control() {
}