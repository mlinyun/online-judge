#include "DiscussList.h"
#include "MongoDataBase.h"

DiscussList *DiscussList::GetInstance() {
    static DiscussList disscusslist;
    return &disscusslist;
}

// 添加讨论
Json::Value DiscussList::InsertDiscuss(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertDiscuss(insertjson);
}

// 分页查询讨论
Json::Value DiscussList::SelectDiscussList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussList(queryjson);
}

// 管理员分页查询
Json::Value DiscussList::SelectDiscussListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

// 查询
Json::Value DiscussList::SelectDiscussByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussByEdit(queryjson);
}

// 查询讨论内容
Json::Value DiscussList::SelectDiscuss(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscuss(queryjson);
}

// 修改讨论的评论数
bool DiscussList::UpdateDiscussComments(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscussComments(updatejson);
}

// 更新讨论
Json::Value DiscussList::UpdateDiscuss(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscuss(updatejson);
}

// 删除讨论
Json::Value DiscussList::DeleteDiscuss(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteDiscuss(deletejson);
}

DiscussList::DiscussList() {
}

DiscussList::~DiscussList() {
}