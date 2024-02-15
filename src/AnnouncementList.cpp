#include "AnnouncementList.h"
#include "MongoDataBase.h"

// 局部静态特性的方式实现单实例
AnnouncementList *AnnouncementList::GetInstance() {
    static AnnouncementList announcementlist;
    return &announcementlist;
}

AnnouncementList::AnnouncementList() {
}

AnnouncementList::~AnnouncementList() {
}