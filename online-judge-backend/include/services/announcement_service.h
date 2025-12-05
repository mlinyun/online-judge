#ifndef ANNOUNCEMENT_SERVICE_H
#define ANNOUNCEMENT_SERVICE_H

#include <json/json.h>

/**
 * 公告服务类头文件
 */
class AnnouncementService {
private:
    AnnouncementService();

    ~AnnouncementService();

public:
    // 局部静态特性的方式实现单实例模式
    static AnnouncementService *GetInstance();

    // 添加公告（管理员权限）
    Json::Value InsertAnnouncement(Json::Value &insertjson);

    // 查询公告详细信息，并将其浏览量加 1
    Json::Value SelectAnnouncement(Json::Value &queryjson);

    // 查询公告的详细信息，主要是编辑时的查询（管理员权限）
    Json::Value SelectAnnouncementByEdit(Json::Value &queryjson);

    // 更新公告（管理员权限）
    Json::Value UpdateAnnouncement(Json::Value &updatejson);

    // 删除公告（管理员权限）
    Json::Value DeleteAnnouncement(Json::Value &deletejson);

    // 分页查询公告列表
    Json::Value SelectAnnouncementList(Json::Value &queryjson);

    // 分页查询公告列表（管理员权限）
    Json::Value SelectAnnouncementListByAdmin(Json::Value &queryjson);
};

#endif  // ANNOUNCEMENT_SERVICE_H