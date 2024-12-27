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

// ++++++++++++++++++++++++++++++ 用户表模块 Start ++++++++++++++++++++++++++++++
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
// ++++++++++++++++++++++++++++++ 用户表模块 End ++++++++++++++++++++++++++++++

Control::Control()
{
}

Control::~Control()
{
}