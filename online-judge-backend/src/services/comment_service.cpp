#include "services/comment_service.h"

#include "db/mongo_database.h"  // MongoDB 数据库操作类
#include "db/redis_database.h"  // Redis 数据库操作类
#include "utils/response.h"     // 统一响应工具

// 局部静态特性的方式实现单实例模式
CommentService *CommentService::GetInstance() {
    static CommentService comment_service;
    return &comment_service;
}

// 插入父评论
Json::Value CommentService::InsertFatherComment(Json::Value &insertjson) {
    // 从 Token 中获取 UserId 并设置到 insertjson 中
    std::string token = insertjson["Token"].asString();
    std::string userId = ReDB::GetInstance()->GetUserIdByToken(token);
    // 如果获取不到用户 ID，则返回用户不存在
    if (userId.empty()) {
        return response::UserNotFound();
    }
    insertjson["UserId"] = userId;
    return MoDB::GetInstance()->InsertFatherComment(insertjson);
}

// 插入子评论
Json::Value CommentService::InsertSonComment(Json::Value &insertjson) {
    // 从 Token 中获取 UserId 并设置到 insertjson 中
    std::string token = insertjson["Token"].asString();
    std::string userId = ReDB::GetInstance()->GetUserIdByToken(token);
    // 如果获取不到用户 ID，则返回用户不存在
    if (userId.empty()) {
        return response::UserNotFound();
    }
    insertjson["UserId"] = userId;
    return MoDB::GetInstance()->InsertSonComment(insertjson);
}

// 获取父评论
Json::Value CommentService::GetFatherComment(Json::Value &queryjson) {
    return MoDB::GetInstance()->GetFatherComment(queryjson);
}

// 获取子评论
Json::Value CommentService::GetSonComment(Json::Value &queryjson) {
    return MoDB::GetInstance()->GetSonComment(queryjson);
}

// 管理员查询评论
Json::Value CommentService::SelectCommentListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 删除父评论
Json::Value CommentService::DeleteFatherComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteFatherComment(deletejson);
}

// 删除子评论
Json::Value CommentService::DeleteSonComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteSonComment(deletejson);
}

// 删除某一篇文章的所有评论
bool CommentService::DeleteArticleComment(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteArticleComment(deletejson);
}

// 获取评论的作者 UserId
std::string CommentService::GetCommentAuthorId(int64_t commentId) {
    return MoDB::GetInstance()->GetCommentAuthorId(commentId);
}

CommentService::CommentService() {
    // 构造函数实现
}

CommentService::~CommentService() {
    // 析构函数实现
}