#include "AnnouncementList.h"
#include "MongoDataBase.h"

// 局部静态特性的方式实现单实例
AnnouncementList *AnnouncementList::GetInstance() {
    static AnnouncementList announcementlist;
    return &announcementlist;
}

Json::Value AnnouncementList::InsertAnnouncement(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertAnnouncement(insertjson);
}

Json::Value AnnouncementList::SelectAnnouncementList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

Json::Value AnnouncementList::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementList(queryjson);
}

Json::Value AnnouncementList::SelectAnnouncementByEdit(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncementByEdit(queryjson);
}

Json::Value AnnouncementList::SelectAnnouncement(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectAnnouncement(queryjson);
}

bool AnnouncementList::UpdateAnnouncementComments(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateAnnouncementComments(updatejson);
}

Json::Value AnnouncementList::UpdateAnnouncement(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateAnnouncement(updatejson);
}

Json::Value AnnouncementList::DeleteAnnouncement(Json::Value &deletejson) {
    return MoDB::GetInstance()->DeleteAnnouncement(deletejson);
}

AnnouncementList::AnnouncementList() {
}

AnnouncementList::~AnnouncementList() {
}