#include "services/comment_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
CommentService *CommentService::GetInstance() {
    static CommentService comment_service;
    return &comment_service;
}

// 管理员查询评论
Json::Value CommentService::SelectCommentListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 获取父评论
Json::Value CommentService::getFatherComment(Json::Value &queryjson) {
    return MoDB::GetInstance()->getFatherComment(queryjson);
}

// 获取子评论
Json::Value CommentService::getSonComment(Json::Value &queryjson) {
    return MoDB::GetInstance()->getSonComment(queryjson);
}

// 插入父评论
Json::Value CommentService::InsertFatherComment(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertFatherComment(insertjson);
}

// 插入子评论
Json::Value CommentService::InsertSonComment(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertSonComment(insertjson);
}

// 删除某一篇文章的所有评论
bool CommentService::DeleteArticleComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteArticleComment(deletejson);
}

// 删除父评论
Json::Value CommentService::DeleteFatherComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteFatherComment(deletejson);
}

// 删除子评论
Json::Value CommentService::DeleteSonComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteSonComment(deletejson);
}

CommentService::CommentService() {
    // 构造函数实现
}

CommentService::~CommentService() {
    // 析构函数实现
}