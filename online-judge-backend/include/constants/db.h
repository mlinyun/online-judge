#ifndef CONSTANTS_DB_H
#define CONSTANTS_DB_H

#include <string>

/**
 * 数据库相关常量
 */
namespace constants {
namespace db {
/**
 * MongoDB 配置
 */
// MongoDB 数据库名称
constexpr const char* DATABASE_NAME = "online_judge";
// MongoDB 连接字符串
constexpr const char* MONGO_URI = "mongodb://admin:StrongPassw0rd!@localhost:27017/?authSource=admin";

/**
 * MongoDB 集合名称
 */
// 公告集合
constexpr const char* COLLECTION_ANNOUNCEMENTS = "announcements";
// 评论集合
constexpr const char* COLLECTION_COMMENTS = "comments";
// 讨论集合
constexpr const char* COLLECTION_DISCUSSES = "discusses";
// 题目集合
constexpr const char* COLLECTION_PROBLEMS = "problems";
// 题解集合
constexpr const char* COLLECTION_SOLUTIONS = "solutions";
// 状态记录集合
constexpr const char* COLLECTION_STATUS_RECORDS = "status_records";
// 标签集合
constexpr const char* COLLECTION_TAGS = "tags";
// 用户集合
constexpr const char* COLLECTION_USERS = "users";

/**
 * Redis 配置
 */
// Redis 服务器地址
constexpr const char* REDIS_HOST = "127.0.0.1";
// Redis 服务器端口
constexpr int REDIS_PORT = 6379;
}  // namespace db
}  // namespace constants

#endif  // CONSTANTS_DB_H
