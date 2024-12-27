#ifndef PROBLEMLIST_H
#define PROBLEMLIST_H

#include <map>
#include <string>
#include <jsoncpp/json/json.h>

// 题目类
class ProblemList
{
public:
    // 局部静态特性的方式实现单实例
    static ProblemList *GetInstance();

    // 管理员查询题目详细信息
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    // 用户查询题目详细信息
    Json::Value SelectProblem(Json::Value &queryjson);

    // 插入题目
    Json::Value InsertProblem(Json::Value &insertjson);

    // 修改题目信息
    Json::Value UpdateProblem(Json::Value &updatejson);

    // 删除题目
    Json::Value DeleteProblem(Json::Value &deletejson);

    // 通过普通查询获取题库数据
    Json::Value SelectProblemList(Json::Value &queryjson);

    // 管理员查询列表
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    // 更新题目的状态数量
    bool UpdateProblemStatusNum(Json::Value &updatejson);

private:
    ProblemList();

    ~ProblemList();
};

#endif
