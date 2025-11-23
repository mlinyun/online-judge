#ifndef DISCUSS_SERVICE_H
#define DISCUSS_SERVICE_H

/**
 * 讨论服务类头文件
 */
class DiscussService {
private:
    DiscussService();

    ~DiscussService();

public:
    // 局部静态特性的方式实现单实例模式
    static DiscussService *GetInstance();
};

#endif  // DISCUSS_SERVICE_H