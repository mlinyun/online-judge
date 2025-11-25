#ifndef PROBLEM_SERVICE_H
#define PROBLEM_SERVICE_H

#include <json/json.h>

/**
 * 题目服务类头文件
 */
class ProblemService {
private:
    ProblemService();

    ~ProblemService();

public:
    // 局部静态特性的方式实现单实例模式
    static ProblemService *GetInstance();

    // 查询题目信息（单条）
    Json::Value SelectProblem(Json::Value &queryjson);

    // 查询题目信息（管理员权限）
    Json::Value SelectProblemInfoByAdmin(Json::Value &queryjson);

    // 插入题目（管理员权限）
    Json::Value InsertProblem(Json::Value &insertjson);

    // 更新题目信息（管理员权限）
    Json::Value UpdateProblem(Json::Value &updatejson);

    // 删除题目（管理员权限）
    Json::Value DeleteProblem(Json::Value &deletejson);

    // 分页获取题目列表
    Json::Value SelectProblemList(Json::Value &queryjson);

    // 分页获取题目列表（管理员权限）
    Json::Value SelectProblemListByAdmin(Json::Value &queryjson);

    // 更新题目的状态数量
    bool UpdateProblemStatusNum(Json::Value &updatejson);
};

#endif  // PROBLEM_SERVICE_H