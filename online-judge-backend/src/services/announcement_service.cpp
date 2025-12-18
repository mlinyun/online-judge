#include "services/announcement_service.h"

#include "db/mongo_database.h"  // MongoDB 数据库操作类
#include "db/redis_database.h"  // Redis 数据库操作类
#include "utils/response.h"     // 统一响应工具

// 局部静态特性的方式实现单实例模式
AnnouncementService *AnnouncementService::GetInstance() {
    static AnnouncementService announcement_service;
    return &announcement_service;
}

// 添加公告（管理员权限）
Json::Value AnnouncementService::InsertAnnouncement(Json::Value &insertjson) {
    // 从 Token 中获取 UserId 并设置到 insertjson 中
    std::string token = insertjson["Token"].asString();
    std::string userId = ReDB::GetInstance()->GetUserIdByToken(token);
    // 如果获取不到用户 ID，则返回用户不存在
    if (userId.empty()) {
        return response::UserNotFound();
    }
    insertjson["UserId"] = userId;
    return MoDB::GetInstance()->InsertAnnouncement(insertjson);
}

// 查询公告详细信息，并将其浏览量加 1
Json::Value AnnouncementService::SelectAnnouncement(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncement(queryjson);
}

// 查询公告的详细信息，主要是编辑时的查询
Json::Value AnnouncementService::SelectAnnouncementByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementByEdit(queryjson);
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
    return MoDB::GetInstance()->SelectAnnouncementListByAdmin(queryjson);
}

// 设置公告激活状态（管理员权限）
Json::Value AnnouncementService::UpdateAnnouncementActive(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateAnnouncementActive(updatejson);
}

AnnouncementService::AnnouncementService() {
    // 构造函数实现
}

AnnouncementService::~AnnouncementService() {
    // 析构函数实现
}
