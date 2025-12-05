#include "services/discuss_service.h"

#include "db/mongo_database.h"  // MongoDB 数据库操作类
#include "db/redis_database.h"  // Redis 数据库操作类
#include "utils/response.h"     // 统一响应工具

// 局部静态特性的方式实现单实例模式
DiscussService *DiscussService::GetInstance() {
    static DiscussService discuss_service;
    return &discuss_service;
}

// 添加讨论
Json::Value DiscussService::InsertDiscuss(Json::Value &insertjson) {
    // 从 Token 中获取 UserId 并设置到 insertjson 中
    std::string token = insertjson["Token"].asString();
    std::string userId = ReDB::GetInstance()->GetUserIdByToken(token);
    // 如果获取不到用户 ID，则返回用户不存在
    if (userId.empty()) {
        return response::UserNotFound();
    }
    insertjson["UserId"] = userId;
    return MoDB::GetInstance()->InsertDiscuss(insertjson);
}

// 查询讨论的详细内容，并且将其浏览量加 1
Json::Value DiscussService::SelectDiscuss(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscuss(queryjson);
}

// 查询讨论的详细信息，主要是编辑时的查询
Json::Value DiscussService::SelectDiscussByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussByEdit(queryjson);
}

// 更新讨论
Json::Value DiscussService::UpdateDiscuss(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscuss(updatejson);
}

// 删除讨论
Json::Value DiscussService::DeleteDiscuss(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteDiscuss(deletejson);
}

// 分页查询讨论
Json::Value DiscussService::SelectDiscussList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussList(queryjson);
}

// 分页查询讨论（管理员权限）
Json::Value DiscussService::SelectDiscussListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

// 获取讨论的作者 UserId
std::string DiscussService::GetDiscussAuthorId(int64_t discussId) {
    return MoDB::GetInstance()->GetDiscussAuthorId(discussId);
}

DiscussService::DiscussService() {
    // 构造函数实现
}

DiscussService::~DiscussService() {
    // 析构函数实现
}