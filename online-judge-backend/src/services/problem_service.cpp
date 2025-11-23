#include "services/problem_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
ProblemService* ProblemService::GetInstance() {
    static ProblemService problem_service;
    return &problem_service;
}

ProblemService::ProblemService() {
    // 构造函数实现
}

ProblemService::~ProblemService() {
    // 析构函数实现
}