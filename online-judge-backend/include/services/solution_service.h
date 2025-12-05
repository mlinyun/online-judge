#ifndef SOLUTION_SERVICE_H
#define SOLUTION_SERVICE_H

#include <json/json.h>

/**
 * 题解服务类头文件
 */
class SolutionService {
private:
    SolutionService();

    ~SolutionService();

public:
    // 局部静态特性的方式实现单实例模式
    static SolutionService *GetInstance();

    // 添加题解
    Json::Value InsertSolution(Json::Value &insertjson);

    // 查询题解的详细内容，并且将其浏览量加 1
    Json::Value SelectSolution(Json::Value &queryjson);

    // 查询题解的详细信息，主要是编辑时的查询
    Json::Value SelectSolutionByEdit(Json::Value &queryjson);

    // 更新题解
    Json::Value UpdateSolution(Json::Value &updatejson);

    // 删除题解
    Json::Value DeleteSolution(Json::Value &deletejson);

    // 分页查询题解（公开题解）
    Json::Value SelectSolutionList(Json::Value &queryjson);

    // 分页查询题解（管理员权限）
    Json::Value SelectSolutionListByAdmin(Json::Value &queryjson);

    /**
     * 功能：获取题解的作者 UserId
     * 传入：题解 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetSolutionAuthorId(int64_t solutionId);
};

#endif  // SOLUTION_SERVICE_H