#include "db/redis_database.h"

#include "constants/db.h"

using namespace std;

// 局部静态特性的方式实现单实例模式
ReDB *ReDB::GetInstance() {
    static ReDB redis_database;
    return &redis_database;
}

// 存放 Token
bool ReDB::SetToken(std::string token, std::string userid) {
    try {
        // 设置 token，有效期为 7 天（604800 秒）
        redis_token->setex(token, 604800, userid);
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

// 通过 Token 取出用户 ID
std::string ReDB::GetUserIdByToken(std::string token) {
    try {
        auto res = redis_token->get(token);
        if (res)
            return *res;
        else
            return "0";
    } catch (const std::exception &e) {
        return "0";
    }
}

ReDB::ReDB() {
    // 构造函数实现
    // 创建 Redis 连接配置
    ConnectionOptions connection_opts;
    connection_opts.host = constants::db::REDIS_HOST;          // Redis 服务器地址
    connection_opts.port = constants::db::REDIS_PORT;          // Redis 服务器端口
    connection_opts.password = constants::db::REDIS_PASSWORD;  // Redis 连接密码
    connection_opts.db = constants::db::REDIS_DB_INDEX;        // Redis 数据库索引
    // 连接超时设置
    connection_opts.socket_timeout = std::chrono::milliseconds{constants::db::REDIS_SOCKET_TIMEOUT_MS};
    // 连接建立超时设置
    connection_opts.connect_timeout = std::chrono::milliseconds{constants::db::REDIS_CONNECTION_TIMEOUT_MS};

    // 创建 Redis 实例
    redis_token = new Redis(connection_opts);
}

ReDB::~ReDB() {
    // 析构函数实现
}