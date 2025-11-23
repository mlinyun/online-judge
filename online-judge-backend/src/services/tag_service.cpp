#include "services/tag_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
TagService* TagService::GetInstance() {
    static TagService tag_service;
    return &tag_service;
}

TagService::TagService() {
    // 构造函数实现
}

TagService::~TagService() {
    // 析构函数实现
}