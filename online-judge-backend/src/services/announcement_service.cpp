#include "services/announcement_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
AnnouncementService *AnnouncementService::GetInstance() {
    static AnnouncementService announcement_service;
    return &announcement_service;
}

// 添加公告（管理员权限）
Json::Value AnnouncementService::InsertAnnouncement(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertAnnouncement(insertjson);
}

// 查询公告详细信息，并将其浏览量加 1
Json::Value AnnouncementService::SelectAnnouncement(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncement(queryjson);
}

// 更新公告（管理员权限）
Json::Value AnnouncementService::UpdateAnnouncement(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateAnnouncement(updatejson);
}

// 删除公告（管理员权限）
Json::Value AnnouncementService::DeleteAnnouncement(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteAnnouncement(deletejson);
}

// 分页查询公告列表
Json::Value AnnouncementService::SelectAnnouncementList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

// 分页查询公告列表（管理员权限）
Json::Value AnnouncementService::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

// 查询公告的详细信息，主要是编辑时的查询
Json::Value AnnouncementService::SelectAnnouncementByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

// 更新公告的评论数量
bool AnnouncementService::UpdateAnnouncementComments(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateAnnouncementComments(updatejson);
}

AnnouncementService::AnnouncementService() {
    // 构造函数实现
}

AnnouncementService::~AnnouncementService() {
    // 析构函数实现
}