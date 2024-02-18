#ifndef SOLUTIONLIST_H
#define SOLUTIONLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 讨论类
class SolutionList {
public:
    // 局部静态特性的方式实现单实例
    static SolutionList *GetInstance();

    // 添加题解
    Json::Value InsertSolution(Json::Value &insertjson);

    // 分页查询题解
    Json::Value SelectSolutionList(Json::Value &queryjson);

    // 管理员查询
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);

    // 查询
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);

    // 查询题解的内容
    Json::Value SelectSolution(Json::Value &queryjson);

    // 修改评论数的数量
    bool UpdateSolutionComments(Json::Value &updatejson);

    // 更新题解
    Json::Value UpdateSolution(Json::Value &updatejson);

    // 删除题解
    Json::Value DeleteSolution(Json::Value &deletejson);

private:
    SolutionList();

    ~SolutionList();
};

#endif