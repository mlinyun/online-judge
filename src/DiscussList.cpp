#include "DiscussList.h"
#include "MongoDataBase.h"

DiscussList *DiscussList::GetInstance() {
    static DiscussList disscusslist;
    return &disscusslist;
}

// 插入题解
Json::Value DiscussList::InsertDiscuss(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertDiscuss(insertjson);
}

// 查询题解列表
Json::Value DiscussList::SelectDiscussList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussList(queryjson);
}

// 管理员查询
Json::Value DiscussList::SelectDiscussListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussListByAdmin(queryjson);
}

// 查询题解进行编辑
Json::Value DiscussList::SelectDiscussByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscussByEdit(queryjson);
}

// 查询题解
Json::Value DiscussList::SelectDiscuss(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectDiscuss(queryjson);
}

// 修改讨论的评论数
bool DiscussList::UpdateDiscussComments(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscussComments(updatejson);
}

// 更新题解
Json::Value DiscussList::UpdateDiscuss(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateDiscuss(updatejson);
}

// 删除题解
Json::Value DiscussList::DeleteDiscuss(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteDiscuss(deletejson);
}

DiscussList::DiscussList() {
}

DiscussList::~DiscussList() {
}