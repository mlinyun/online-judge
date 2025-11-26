#ifndef CONTROL_H
#define CONTROL_H

#include <json/json.h>

/**
 * 控制类头文件
 */
class Control {
private:
    /* data */
public:
    // ------------------------------ 用户模块 Start ------------------------------
    // 注册用户
    Json::Value RegisterUser(Json::Value &registerjson);

    // 登录用户
    Json::Value LoginUser(Json::Value &loginjson);

    // 查询用户信息
    Json::Value SelectUserInfo(Json::Value &queryjson);

    // 查询用户信息（在设置页面修改用户时使用）
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    // 更新用户信息
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    // 删除用户
    Json::Value DeleteUser(Json::Value &deletejson);

    // 用户排名查询
    Json::Value SelectUserRank(Json::Value &queryjson);

    // 分页查询用户列表（管理员权限）
    Json::Value SelectUserSetInfo(Json::Value &queryjson);
    // ------------------------------ 用户模块 End ------------------------------

    // ------------------------------ 题目模块 Start ------------------------------
    // 查询题目信息（单条）
    Json::Value SelectProblem(Json::Value &queryjson);

    // 查询题目信息（管理员权限）
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    // 编辑题目：包含插入和更新题目（管理员权限）
    Json::Value EditProblem(Json::Value &insertjson);

    // 删除题目（管理员权限）
    Json::Value DeleteProblem(Json::Value &deletejson);

    // 分页获取题目列表
    Json::Value SelectProblemList(Json::Value &queryjson);

    // 分页获取题目列表（管理员权限）
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);
    // ------------------------------ 题目模块 End ------------------------------

    // ------------------------------ 标签模块 Start ------------------------------
    // 获取题目的所有标签
    Json::Value GetTags(Json::Value &queryjson);
    // ------------------------------ 标签模块 End ------------------------------

    // ------------------------------ 公告模块 Start ------------------------------
    // 添加公告（管理员权限）
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    // 查询公告详细信息，并将其浏览量加 1
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    // 更新公告（管理员权限）
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    // 删除公告（管理员权限）
    Json::Value DeleteAnnouncement(Json::Value &deletejson);

    // 分页查询公告列表
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    // 分页查询公告列表（管理员权限）
    Json::Value SelectAnnouncementListByAdmin(Json::Value &queryjson);

    // 查询公告的详细信息，主要是编辑时的查询
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);
    // ------------------------------ 公告模块 End ------------------------------

    // ------------------------------ 讨论模块 Start ------------------------------
    // 添加讨论
    Json::Value InsertDiscuss(Json::Value &insertjson);

    // 查询讨论的详细内容，并且将其浏览量加 1
    Json::Value SelectDiscuss(Json::Value &queryjson);

    // 更新讨论
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    // 删除讨论
    Json::Value DeleteDiscuss(Json::Value &deletejson);

    // 分页查询讨论
    Json::Value SelectDiscussList(Json::Value &queryjson);

    // 分页查询讨论（管理员权限）
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    // 查询讨论的详细信息，主要是编辑时的查询
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);
    // ------------------------------ 讨论模块 End ------------------------------

    // ------------------------------ 题解模块 Start ------------------------------
    // 添加题解
    Json::Value InsertSolution(Json::Value &insertjson);

    // 查询题解的详细内容，并且将其浏览量加 1
    Json::Value SelectSolution(Json::Value &queryjson);

    // 更新题解
    Json::Value UpdateSolution(Json::Value &updatejson);

    // 删除题解
    Json::Value DeleteSolution(Json::Value &deletejson);

    // 分页查询题解（公开题解）
    Json::Value SelectSolutionList(Json::Value &queryjson);

    // 分页查询题解（管理员权限）
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);

    // 查询题解的详细信息，主要是编辑时的查询
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);
    // ------------------------------ 题解模块 End ------------------------------

    // ------------------------------ 评论模块 Start ------------------------------
    // 管理员查询评论
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    // 获取评论 根据Id
    Json::Value GetComment(Json::Value &queryjson);

    // 插入评论
    Json::Value InsertComment(Json::Value &insertjson);

    // 删除评论
    Json::Value DeleteComment(Json::Value &deletejson);
    // ------------------------------ 评论模块 End ------------------------------

    // ------------------------------ 测评记录模块 Start ------------------------------
    // 返回状态记录的信息
    Json::Value SelectStatusRecordList(Json::Value &queryjson);

    // 查询一条详细测评记录
    Json::Value SelectStatusRecord(Json::Value &queryjson);
    // ------------------------------ 测评记录模块 End ------------------------------

    // ------------------------------ 判题模块 Start ------------------------------
    // 返回判题信息
    Json::Value GetJudgeCode(Json::Value judgejson);
    // ------------------------------ 判题模块 End ------------------------------

    Control();

    ~Control();
};

#endif  // CONTROL_H