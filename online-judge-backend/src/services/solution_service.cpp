#include "services/solution_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
SolutionService* SolutionService::GetInstance() {
    static SolutionService solution_service;
    return &solution_service;
}

SolutionService::SolutionService() {
    // 构造函数实现
}

SolutionService::~SolutionService() {
    // 析构函数实现
}