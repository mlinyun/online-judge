#ifndef ANNOUNCEMENTLIST_H
#define ANNOUNCEMENTLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 公告类
class AnnouncementList {
public:
    // 局部静态特性的方式实现单实例
    static AnnouncementList *GetInstance();

private:
    AnnouncementList();

    ~AnnouncementList();
};

#endif