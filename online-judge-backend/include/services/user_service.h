#ifndef USER_SERVICE_H
#define USER_SERVICE_H

/**
 * 用户服务类头文件
 */
class UserService {
private:
    UserService();

    ~UserService();

public:
    // 局部静态特性的方式实现单实例模式
    static UserService *GetInstance();
};

#endif  // USER_SERVICE_H