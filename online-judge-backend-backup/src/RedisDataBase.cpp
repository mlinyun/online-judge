#include "RedisDataBase.h"

using namespace std;

ReDB *ReDB::GetInstance()
{
    static ReDB redb;
    return &redb;
}

// 存放 Token
bool ReDB::SetToken(std::string token, std::string userid)
{
    try
    {
        redis_token->setex(token, 604800, userid);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 通过 Token 取出用户 ID
std::string ReDB::GetUserIdByToken(std::string token)
{
    try
    {
        auto res = redis_token->get(token);
        if (res)
            return *res;
        else
            return "0";
    }
    catch (const std::exception &e)
    {
        return "0";
    }
}

// 添加题目缓存
bool ReDB::AddProblemCache(std::string problemid, std::string probleminfo)
{
    string id = "Cache:Problem:" + problemid;
    try
    {
        redis_cache->setex(id, 86400, probleminfo);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 获取题目缓存
std::string ReDB::GetProblemCache(std::string problemid)
{
    string id = "Cache:Problem:" + problemid;
    try
    {
        auto res = redis_cache->get(id);
        if (res)
            return *res;
        else
            return "";
    }
    catch (const std::exception &e)
    {
        return "";
    }
}

// 删除题目缓存（当题目修改）
bool ReDB::DeleteProblemCache(std::string problemid)
{
    string id = "Cache:Problem:" + problemid;
    try
    {
        return redis_cache->del(id);
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 添加测评记录缓存
bool ReDB::AddStatusRecordCache(std::string statusrecordid, std::string statusrecordinfo)
{
    string id = "Cache:StatusRecord:" + statusrecordid;
    try
    {
        redis_cache->setex(id, 86400, statusrecordinfo);
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

// 获取测评记录缓存
std::string ReDB::GetStatusRecordCache(std::string statusrecordid)
{
    string id = "Cache:StatusRecord:" + statusrecordid;
    try
    {
        auto res = redis_cache->get(id);
        if (res)
            return *res;
        else
            return "";
    }
    catch (const std::exception &e)
    {
        return "";
    }
}

ReDB::ReDB()
{
    redis_token = new Redis("tcp://127.0.0.1:6379/0?socket_timeout=50ms&connect_timeout=1s&pool_size=5");
    redis_cache = new Redis("tcp://127.0.0.1:6379/1?socket_timeout=50ms&connect_timeout=1s&pool_size=5");
}

ReDB::~ReDB()
{
}
