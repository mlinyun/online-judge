#include "services/announcement_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
AnnouncementService* AnnouncementService::GetInstance() {
    static AnnouncementService announcement_service;
    return &announcement_service;
}

AnnouncementService::AnnouncementService() {
    // 构造函数实现
}

AnnouncementService::~AnnouncementService() {
    // 析构函数实现
}