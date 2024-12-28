#include "AnnouncementList.h"
#include "MongoDataBase.h"

// 局部静态特性的方式实现单实例
AnnouncementList *AnnouncementList::GetInstance()
{
    static AnnouncementList announcementlist;
    return &announcementlist;
}

// 添加公告
Json::Value AnnouncementList::InsertAnnouncement(Json::Value &insertjson)
{
    return MoDB::GetInstance()->InsertAnnouncement(insertjson);
}

// 分页查询公告
Json::Value AnnouncementList::SelectAnnouncementList(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

// 管理员分页查询公告
Json::Value AnnouncementList::SelectAnnouncementListByAdmin(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

// 查询公告的详细信息
Json::Value AnnouncementList::SelectAnnouncementByEdit(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

// 查询公告的内容
Json::Value AnnouncementList::SelectAnnouncement(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectAnnouncement(queryjson);
}

// 修改评论数的数量
bool AnnouncementList::UpdateAnnouncementComments(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateAnnouncementComments(updatejson);
}

// 更新公告
Json::Value AnnouncementList::UpdateAnnouncement(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateAnnouncement(updatejson);
}

// 删除公告
Json::Value AnnouncementList::DeleteAnnouncement(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteAnnouncement(deletejson);
}

AnnouncementList::AnnouncementList()
{
}

AnnouncementList::~AnnouncementList()
{
}