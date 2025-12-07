#include "db/redis_database.h"

#include <unordered_set>

#include "constants/db.h"

using namespace std;

// 局部静态特性的方式实现单实例模式
ReDB *ReDB::GetInstance() {
    static ReDB redis_database;
    return &redis_database;
}

/**
 * 功能：设置 Token 对应的用户 ID
 * 
 * 每次登录时，先删除该用户的所有旧 Token，再创建新 Token，这样确保同一用户同一时间只有一个有效 Token
 * @param token Token 字符串
 * @param userid 用户 ID 字符串
 * @return 是否设置成功
 */
bool ReDB::SetToken(std::string token, std::string userid) {
    try {
        // 1. 先删除该用户的所有旧 Token
        DeleteTokensByUserId(userid);

        // 2. 设置新 token，有效期为 7 天（604800 秒）
        redis_token->setex(token, 604800, userid);  // 键为 token，值为 userid

        // 3. 同时将 token 添加到用户的 token 集合中，方便后续通过 userid 删除所有 token
        std::string user_tokens_key = "UserTokens:" + userid;
        redis_token->sadd(user_tokens_key, token);
        // 设置用户 token 集合的过期时间为 7 天
        redis_token->expire(user_tokens_key, 604800);

        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

// 通过 Token 取出用户 ID
std::string ReDB::GetUserIdByToken(std::string token) {
    try {
        // 获取 token 对应的 userid
        auto res = redis_token->get(token);
        if (res) {
            // 如果存在则返回 userid
            return *res;
        } else {
            // 不存在则返回 "0"
            return "0";
        }
    } catch (const std::exception &e) {
        return "0";
    }
}

// 删除某个用户的所有 Token 记录
bool ReDB::DeleteTokensByUserId(std::string userid) {
    try {
        std::string user_tokens_key = "UserTokens:" + userid;

        // 获取用户的所有 token
        std::unordered_set<std::string> tokens;
        redis_token->smembers(user_tokens_key, std::inserter(tokens, tokens.begin()));

        // 删除所有 token
        for (const auto &token : tokens) {
            redis_token->del(token);
        }

        // 删除用户的 token 集合
        redis_token->del(user_tokens_key);

        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

// 添加题目缓存
bool ReDB::AddProblemCache(std::string problemid, std::string probleminfo) {
    string id = "Cache:Problem:" + problemid;
    try {
        redis_cache->setex(id, 86400, probleminfo);
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

// 获取题目缓存
std::string ReDB::GetProblemCache(std::string problemid) {
    string id = "Cache:Problem:" + problemid;
    try {
        auto res = redis_cache->get(id);
        if (res) {
            return *res;
        } else {
            return "";
        }
    } catch (const std::exception &e) {
        return "";
    }
}

// 删除题目缓存（当题目修改）
bool ReDB::DeleteProblemCache(std::string problemid) {
    string id = "Cache:Problem:" + problemid;
    try {
        return redis_cache->del(id);
    } catch (const std::exception &e) {
        return false;
    }
}

// 添加测评记录缓存
bool ReDB::AddStatusRecordCache(std::string statusrecordid, std::string statusrecordinfo) {
    string id = "Cache:StatusRecord:" + statusrecordid;
    try {
        redis_cache->setex(id, 86400, statusrecordinfo);
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

// 获取测评记录缓存
std::string ReDB::GetStatusRecordCache(std::string statusrecordid) {
    string id = "Cache:StatusRecord:" + statusrecordid;
    try {
        auto res = redis_cache->get(id);
        if (res) {
            return *res;
        } else {
            return "";
        }
    } catch (const std::exception &e) {
        return "";
    }
}

ReDB::ReDB() {
    // 构造函数实现
    // 创建通用 Redis 连接配置
    auto build_connection_opts = [](std::int64_t db_index) {
        ConnectionOptions opts;
        opts.host = constants::db::REDIS_HOST;          // Redis 服务器地址
        opts.port = constants::db::REDIS_PORT;          // Redis 服务器端口
        opts.password = constants::db::REDIS_PASSWORD;  // Redis 连接密码
        opts.db = db_index;                             // Redis 数据库索引
        opts.socket_timeout = std::chrono::milliseconds{constants::db::REDIS_SOCKET_TIMEOUT_MS};
        opts.connect_timeout = std::chrono::milliseconds{constants::db::REDIS_CONNECTION_TIMEOUT_MS};
        return opts;
    };

    // 创建 Redis 实例，分别使用不同数据库索引
    redis_token = new Redis(build_connection_opts(constants::db::REDIS_TOKEN_INDEX));  // Token 使用 DB 0
    redis_cache = new Redis(build_connection_opts(constants::db::REDIS_CACHE_INDEX));  // 缓存使用 DB 1
}

ReDB::~ReDB() {
    // 析构函数实现
    delete redis_token;
    delete redis_cache;
}