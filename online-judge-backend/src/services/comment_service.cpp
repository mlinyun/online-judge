#include "services/comment_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
CommentService* CommentService::GetInstance() {
    static CommentService comment_service;
    return &comment_service;
}

CommentService::CommentService() {
    // 构造函数实现
}

CommentService::~CommentService() {
    // 析构函数实现
}