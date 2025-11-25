#include "services/solution_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
SolutionService *SolutionService::GetInstance() {
    static SolutionService solution_service;
    return &solution_service;
}

// 添加题解
Json::Value SolutionService::InsertSolution(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertSolution(insertjson);
}

// 查询题解的详细内容，并且将其浏览量加 1
Json::Value SolutionService::SelectSolution(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolution(queryjson);
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

// 查询题解的详细信息，主要是编辑时的查询
Json::Value SolutionService::SelectSolutionByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectSolutionByEdit(queryjson);
}

SolutionService::SolutionService() {
    // 构造函数实现
}

SolutionService::~SolutionService() {
    // 析构函数实现
}