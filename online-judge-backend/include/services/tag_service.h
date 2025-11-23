#ifndef TAG_SERVICE_H
#define TAG_SERVICE_H

/**
 * 标签服务类头文件
 */
class TagService {
private:
    TagService();

    ~TagService();

public:
    // 局部静态特性的方式实现单实例模式
    static TagService *GetInstance();
};

#endif  // TAG_SERVICE_H