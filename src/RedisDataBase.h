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

    // =============== 题目 ======================
    // 添加题目缓存
    bool AddProblemCache(std::string problemid, std::string probleminfo);

    // 获取题目缓存
    std::string GetProblemCache(std::string problemid);

    // 删除题目缓存（当题目修改，删除时）
    bool DeleteProblemCache(std::string problemid);

    // =============== 测评记录=================
    // 添加测评记录缓存
    bool AddStatusRecordCache(std::string statusrecordid, std::string statusrecordinfo);

    // 获取测评记录缓存
    std::string GetStatusRecordCache(std::string statusrecordid);

private:
    ReDB();

    ~ReDB();

    Redis *redis_token;

    Redis *redis_cache;
};

#endif