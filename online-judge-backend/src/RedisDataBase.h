#ifndef REDISDATABASE_H
#define REDISDATABASE_H

#include <sw/redis++/redis++.h>
#include <string>
using namespace sw::redis;

// Redis 数据库
class ReDB
{
public:
    // 局部静态特性的方式实现单实例
    static ReDB *GetInstance();

    // 存放 Token
    bool SetToken(std::string token, std::string userid);

    // 通过 Token 取出用户 ID
    std::string GetUserIdByToken(std::string token);

private:
    ReDB();

    ~ReDB();

    Redis *redis_token;
};

#endif
