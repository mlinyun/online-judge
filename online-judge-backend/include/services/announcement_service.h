#ifndef ANNOUNCEMENT_SERVICE_H
#define ANNOUNCEMENT_SERVICE_H

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
};

#endif  // ANNOUNCEMENT_SERVICE_H