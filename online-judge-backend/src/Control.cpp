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
}
// ++++++++++++++++++++++++++++++ 题目模块 End ++++++++++++++++++++++++++++++

Control::Control()
{
    // 初始化题目标签
    Tag::GetInstance()->InitProblemTags();
}

Control::~Control()
{
}