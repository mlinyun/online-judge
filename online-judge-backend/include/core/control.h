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
    /**
     * 功能：用户注册
     * 权限：所有用户均可以注册
     */
    Json::Value UserRegister(Json::Value &registerjson);

    /**
     * 功能：用户登录
     * 权限：所有用户均可以登录（被封禁的除外：未实现）
     */
    Json::Value UserLogin(Json::Value &loginjson);

    /**
     * 功能：查询用户信息
     * 权限：只允许用户本人查询
     */
    Json::Value SelectUserInfo(Json::Value &queryjson);

    /**
     * 功能：查询用户信息（在设置页面修改用户时使用）
     * 权限：只允许用户本人查询
     */
    Json::Value SelectUserUpdateInfo(Json::Value &queryjson);

    /**
     * 功能：更新用户信息
     * 权限：只允许用户本人或者管理员修改
     */
    Json::Value UpdateUserInfo(Json::Value &updatejson);

    /**
     * 功能：删除用户
     * 权限：只允许管理员删除
     */
    Json::Value DeleteUser(Json::Value &deletejson);

    /**
     * 功能：用户排名查询
     * 权限：所有用户均可查询
     */
    Json::Value SelectUserRank(Json::Value &queryjson);

    /**
     * 功能：分页查询用户列表
     * 权限：只允许管理员查询
     */
    Json::Value SelectUserSetInfo(Json::Value &queryjson);

    /**
     * 功能：用户登录通过 Token 鉴权
     * 权限：所有用户均可使用
     */
    Json::Value LoginUserByToken(Json::Value &loginjson);
    // ------------------------------ 用户模块 End ------------------------------

    // ------------------------------ 题目模块 Start ------------------------------
    /**
     * 功能：查询题目信息（单条）
     * 权限：所有用户均可查询
     */
    Json::Value SelectProblemInfo(Json::Value &queryjson);

    /**
     * 功能：查询题目信息（单条）
     * 权限：只允许管理员查询
     */
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    /**
     * 功能：编辑题目：包含插入和更新题目
     * 权限：只允许管理员编辑
     */
    Json::Value EditProblem(Json::Value &insertjson);

    /**
     * 功能：删除题目
     * 权限：只允许管理员删除
     */
    Json::Value DeleteProblem(Json::Value &deletejson);

    /**
     * 功能：分页获取题目列表
     * 权限：所有用户均可查询
     */
    Json::Value SelectProblemList(Json::Value &queryjson);

    /**
     * 功能：分页获取题目列表
     * 权限：只允许管理员查询
     */
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);
    // ------------------------------ 题目模块 End ------------------------------

    // ------------------------------ 标签模块 Start ------------------------------
    /**
     * 功能：获取题目的所有标签
     * 权限：所有用户均可查询
     */
    Json::Value GetTags(Json::Value &queryjson);
    // ------------------------------ 标签模块 End ------------------------------

    // ------------------------------ 公告模块 Start ------------------------------
    /**
     * 功能：添加公告
     * 权限：只允许管理员添加
     */
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    /**
     * 功能：查询公告详细信息，并将其浏览量加 1
     * 权限：所有用户均可查询
     */
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    /**
     * 功能：查询公告的详细信息，主要是编辑时的查询
     * 权限：只允许管理员查询
     */
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    /**
     * 功能：更新公告
     * 权限：只允许管理员修改
     */
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    /**
     * 功能：删除公告
     * 权限：只允许管理员删除
     */
    Json::Value DeleteAnnouncement(Json::Value &deletejson);

    /**
     * 功能：分页查询公告列表
     * 权限：所有用户均可查询
     */
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    /**
     * 功能：分页查询公告列表
     * 权限：只允许管理员查询
     */
    Json::Value SelectAnnouncementListByAdmin(Json::Value &queryjson);
    // ------------------------------ 公告模块 End ------------------------------

    // ------------------------------ 讨论模块 Start ------------------------------
    /**
     * 功能：添加讨论
     * 权限：只允许普通用户及以上添加
     */
    Json::Value InsertDiscuss(Json::Value &insertjson);

    /**
     * 功能：查询讨论的详细内容，并且将其浏览量加 1
     * 权限：所有用户均可查询
     */
    Json::Value SelectDiscuss(Json::Value &queryjson);

    /**
     * 功能：查询讨论的详细信息，主要是编辑时的查询
     * 权限：只允许讨论作者本人或者管理员查询
     */
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    /**
     * 功能：更新讨论
     * 权限：只允许讨论作者本人或者管理员修改
     */
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    /**
     * 功能：删除讨论
     * 权限：只允许讨论作者本人或者管理员删除
     */
    Json::Value DeleteDiscuss(Json::Value &deletejson);

    /**
     * 功能：分页查询讨论
     * 权限：所有用户均可查询
     */
    Json::Value SelectDiscussList(Json::Value &queryjson);

    /**
     * 功能：分页查询讨论
     * 权限：只允许管理员查询
     */
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);
    // ------------------------------ 讨论模块 End ------------------------------

    // ------------------------------ 题解模块 Start ------------------------------
    /**
     * 功能：添加题解
     * 权限：只允许普通用户及以上添加
     */
    Json::Value InsertSolution(Json::Value &insertjson);

    /**
     * 功能：查询题解的详细内容，并且将其浏览量加 1
     * 权限：所有用户均可查询
     */
    Json::Value SelectSolution(Json::Value &queryjson);

    /**
     * 功能：查询题解的详细信息，主要是编辑时的查询
     * 权限：只允许题解作者本人或者管理员查询
     */
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);

    /**
     * 功能：更新题解
     * 权限：只允许题解作者本人或者管理员修改
     */
    Json::Value UpdateSolution(Json::Value &updatejson);

    /**
     * 功能：删除题解
     * 权限：只允许题解作者本人或者管理员删除
     */
    Json::Value DeleteSolution(Json::Value &deletejson);

    /**
     * 功能：分页查询题解（公开题解）
     * 权限：所有用户均可查询
     */
    Json::Value SelectSolutionList(Json::Value &queryjson);

    /**
     * 功能：分页查询题解
     * 权限：只允许管理员查询
     */
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);
    // ------------------------------ 题解模块 End ------------------------------

    // ------------------------------ 评论模块 Start ------------------------------
    /**
     * 功能：插入评论
     * 权限：只允许普通用户及以上添加
     */
    Json::Value InsertComment(Json::Value &insertjson);

    /**
     * 功能：获取评论
     * 权限：所有用户均可查询
     */
    Json::Value GetComment(Json::Value &queryjson);

    /**
     * 功能：管理员查询评论
     * 权限：只允许管理员查询
     */
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    /**
     * 功能：删除评论
     * 权限：只允许评论作者本人或者管理员删除
     */
    Json::Value DeleteComment(Json::Value &deletejson);
    // ------------------------------ 评论模块 End ------------------------------

    // ------------------------------ 测评记录模块 Start ------------------------------
    /**
     * 功能：查询一条详细测评记录
     * 权限：只允许状态记录作者本人或者管理员查询
     */
    Json::Value SelectStatusRecord(Json::Value &queryjson);

    /**
     * 功能：返回状态记录的信息
     * 权限：所有用户均可查询
     */
    Json::Value SelectStatusRecordList(Json::Value &queryjson);
    // ------------------------------ 测评记录模块 End ------------------------------

    // ------------------------------ 判题模块 Start ------------------------------
    /**
     * 功能：返回判题信息
     * 权限：只允许普通用户及以上使用
     */
    Json::Value GetJudgeCode(Json::Value judgejson);
    // ------------------------------ 判题模块 End ------------------------------

    Control();

    ~Control();
};

#endif  // CONTROL_H