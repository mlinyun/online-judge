#include "services/discuss_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
DiscussService *DiscussService::GetInstance() {
    static DiscussService discuss_service;
    return &discuss_service;
}

// 添加讨论
Json::Value DiscussService::InsertDiscuss(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertDiscuss(insertjson);
}

// 查询讨论的详细内容，并且将其浏览量加 1
Json::Value DiscussService::SelectDiscuss(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscuss(queryjson);
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

// 查询讨论的详细信息，主要是编辑时的查询
Json::Value DiscussService::SelectDiscussByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussByEdit(queryjson);
}

// 更新讨论的评论数量
bool DiscussService::UpdateDiscussComments(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscussComments(updatejson);
}

DiscussService::DiscussService() {
    // 构造函数实现
}

DiscussService::~DiscussService() {
    // 析构函数实现
}