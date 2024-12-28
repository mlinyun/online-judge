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

ReDB::ReDB()
{
    redis_token = new Redis("tcp://127.0.0.1:6379/0?socket_timeout=50ms&connect_timeout=1s&pool_size=5");
}

ReDB::~ReDB()
{
}
