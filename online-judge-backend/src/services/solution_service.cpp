#include "services/solution_service.h"

#include "db/mongo_database.h"  // MongoDB 数据库操作类
#include "db/redis_database.h"  // Redis 数据库操作类
#include "utils/response.h"     // 统一响应工具

// 局部静态特性的方式实现单实例模式
SolutionService *SolutionService::GetInstance() {
    static SolutionService solution_service;
    return &solution_service;
}

// 添加题解
Json::Value SolutionService::InsertSolution(Json::Value &insertjson) {
    // 从 Token 中获取 UserId 并设置到 insertjson 中
    std::string token = insertjson["Token"].asString();
    std::string userId = ReDB::GetInstance()->GetUserIdByToken(token);
    // 如果获取不到用户 ID，则返回用户不存在
    if (userId.empty()) {
        return response::UserNotFound();
    }
    insertjson["UserId"] = userId;
    return MoDB::GetInstance()->InsertSolution(insertjson);
}

// 查询题解的详细内容，并且将其浏览量加 1
Json::Value SolutionService::SelectSolution(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolution(queryjson);
}

// 查询题解的详细信息，主要是编辑时的查询
Json::Value SolutionService::SelectSolutionByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolutionByEdit(queryjson);
}

// 更新题解
Json::Value SolutionService::UpdateSolution(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateSolution(updatejson);
}

// 删除题解
Json::Value SolutionService::DeleteSolution(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteSolution(deletejson);
}

// 分页查询题解（公开题解）
Json::Value SolutionService::SelectSolutionList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolutionList(queryjson);
}

// 分页查询题解（管理员权限）
Json::Value SolutionService::SelectSolutionListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolutionListByAdmin(queryjson);
}

// 获取题解的作者 UserId
std::string SolutionService::GetSolutionAuthorId(int64_t solutionId) {
    return MoDB::GetInstance()->GetSolutionAuthorId(solutionId);
}

SolutionService::SolutionService() {
    // 构造函数实现
}

SolutionService::~SolutionService() {
    // 析构函数实现
}