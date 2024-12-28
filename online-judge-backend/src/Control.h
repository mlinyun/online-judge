#ifndef CONTROL_H
#define CONTROL_H

#include <jsoncpp/json/json.h>
#include <string>

// 控制类
class Control
{
public:
    // ++++++++++++++++++++ 用户模块 Start ++++++++++++++++++++
    // 注册用户
    Json::Value RegisterUser(Json::Value &registerjson);

    // 登录用户
    Json::Value LoginUser(Json::Value &loginjson);

    // 获取用户大部分信息
    Json::Value SelectUserInfo(Json::Value &queryjson);

    // 更新用户信息
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    // 获取用户信息以供修改
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    // 管理员分页获取用户信息
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    // 删除用户
    Json::Value DeleteUser(Json::Value &deletejson);

    // 获取用户 Rank 排名
    Json::Value SelectUserRank(Json::Value &queryjson);
    // ++++++++++++++++++++ 用户模块 End ++++++++++++++++++++

    // ++++++++++++++++++++ 题目模块 Start ++++++++++++++++++++
    // 管理员查看题目详细信息
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    // 用户查询题目详细信息
    Json::Value SelectProblem(Json::Value &queryjson);

    // 编辑题目
    Json::Value EditProblem(Json::Value &insertjson);

    // 删除题目
    Json::Value DeleteProblem(Json::Value &deletejson);

    // 通过普通查询获取题库数据
    Json::Value SelectProblemList(Json::Value &queryjson);

    // 管理员查询列表
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    // 获取标签
    Json::Value GetTags(Json::Value &queryjson);
    // ++++++++++++++++++++ 题目模块 End ++++++++++++++++++++

    // ++++++++++++++++++++ 公告模块 Start ++++++++++++++++++++
    // 添加公告
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    // 分页查询公告
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    // 管理员分页查询公告
    Json::Value SelectAnnouncementListByAdmin(Json::Value &queryjson);

    // 查询公告的详细信息
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    // 查询公告 进行编辑
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    // 更新公告
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    // 删除公告
    Json::Value DeleteAnnouncement(Json::Value &deletejson);
    // ++++++++++++++++++++ 公告模块 End ++++++++++++++++++++

    // ++++++++++++++++++++ 讨论模块 Start ++++++++++++++++++++
    // 添加讨论
    Json::Value InsertDiscuss(Json::Value &insertjson);

    // 分页查询讨论
    Json::Value SelectDiscussList(Json::Value &queryjson);

    // 管理员分页查询讨论
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    // 查询讨论的详细内容，并且将其浏览量加一
    Json::Value SelectDiscuss(Json::Value &queryjson);

    // 查询讨论的详细信息，主要是编辑时的查询
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    // 修改评论数的数量
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    // 删除讨论
    Json::Value DeleteDiscuss(Json::Value &deletejson);
    // ++++++++++++++++++++ 讨论模块 End ++++++++++++++++++++

    Control();

    ~Control();

private:
};

#endif
