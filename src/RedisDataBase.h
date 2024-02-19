#ifndef REDISDATABASE_H
#define REDISDATABASE_H

#include <sw/redis++/redis++.h>
#include <string>

using namespace sw::redis;

class ReDB {
public:
    // 局部静态特性的方式实现单实例
    static ReDB *GetInstance();

    // ============== Token =======================
    // 存放Token
    bool SetToken(std::string token, std::string userid);

    // 通过Token取出用户ID
    std::string GetUserIdByToken(std::string token);

private:
    ReDB();

    ~ReDB();

    Redis *redis_token;
};

#endif