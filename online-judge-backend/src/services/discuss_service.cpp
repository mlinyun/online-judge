#include "services/discuss_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
DiscussService* DiscussService::GetInstance() {
    static DiscussService discuss_service;
    return &discuss_service;
}

DiscussService::DiscussService() {
    // 构造函数实现
}

DiscussService::~DiscussService() {
    // 析构函数实现
}