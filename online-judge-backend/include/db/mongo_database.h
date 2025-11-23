#ifndef MONGO_DATABASE_H
#define MONGO_DATABASE_H

#include <json/json.h>

#include <atomic>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

using namespace std;

/**
 * MongoDB 数据库操作类头文件
 */
class MoDB {
private:
    static mongocxx::instance instance;  // 该实例应在程序生命周期内存在一次性
    mongocxx::uri uri{};                 // 连接字符串
    mongocxx::pool pool{uri};            // 连接池

    atomic_int64_t m_problem_id;        // 题目 ID 的最大值
    atomic_int64_t m_status_record_id;  // 提交记录 ID 的最大值
    atomic_int64_t m_comment_id;        // 评论 ID 的最大值
    atomic_int64_t m_article_id;        // 文章 ID 的最大值

    /**
     * 功能：获取指定集合的最大 ID
     * 参数：name 集合名称
     * 返回值：最大 ID
     */
    int64_t GetMaxId(string name);

    MoDB();

    ~MoDB();

public:
    // 局部静态特性的方式实现单实例模式
    static MoDB *GetInstance();
};

#endif  // MONGO_DATABASE_H