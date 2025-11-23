#include "services/user_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
UserService* UserService::GetInstance() {
    static UserService user_service;
    return &user_service;
}

UserService::UserService() {
    // 构造函数实现
}

UserService::~UserService() {
    // 析构函数实现
}