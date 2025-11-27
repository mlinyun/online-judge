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
    Redis *redis_token;  // 用于存放 Token 的 Redis 实例
    Redis *redis_cache;  // 用于存放缓存的 Redis 实例

    ReDB();

    ~ReDB();

public:
    // 局部静态特性的方式实现单实例模式
    static ReDB *GetInstance();

    // ------------------- Token Start -------------------
    // 存放 Token
    bool SetToken(string token, string userid);

    // 通过 Token 取出用户 ID
    string GetUserIdByToken(string token);
    // ------------------- Token End -------------------

    // ------------------- 题目 Start -------------------
    // 添加题目缓存
    bool AddProblemCache(string problemid, string probleminfo);

    // 获取题目缓存
    string GetProblemCache(string problemid);

    // 删除题目缓存（当题目修改，删除时）
    bool DeleteProblemCache(string problemid);
    // ------------------- 题目 End -------------------

    // ------------------- 测评记录 Start -------------------
    // 添加测评记录缓存
    bool AddStatusRecordCache(string statusrecordid, string statusrecordinfo);

    // 获取测评记录缓存
    string GetStatusRecordCache(string statusrecordid);
    // ------------------- 测评记录 End -------------------
};

#endif  // REDIS_DATABASE_H