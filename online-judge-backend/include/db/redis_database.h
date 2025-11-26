#ifndef REDIS_DATABASE_H
#define REDIS_DATABASE_H

#include <sw/redis++/redis++.h>

#include <string>

using namespace sw::redis;
using namespace std;

/**
 * Redis 数据库操作类头文件
 */
class ReDB {
private:
    Redis *redis_token;

    ReDB();

    ~ReDB();

public:
    // 局部静态特性的方式实现单实例模式
    static ReDB *GetInstance();

    // 存放 Token
    bool SetToken(std::string token, std::string userid);

    // 通过 Token 取出用户 ID
    string GetUserIdByToken(string token);
};

#endif  // REDIS_DATABASE_H