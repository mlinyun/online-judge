#ifndef MONGODATABASE_H
#define MONGODATABASE_H

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <jsoncpp/json/json.h>
#include <atomic>

class MoDB {
public:
    // 局部静态特性的方式实现单实例
    static MoDB *GetInstance();

private:
    /*
        功能：获取某一个集合中最大的ID
    */
    int64_t GetMaxId(std::string name);

    MoDB();

    ~MoDB();

private:
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri{};           // 连接配置
    mongocxx::pool pool{uri};      // 连接池

    std::atomic_int64_t m_problemid;      // 题目ID
    std::atomic_int64_t m_statusrecordid; // 测评ID
    std::atomic_int64_t m_commentid;      // 评论ID
    std::atomic_int64_t m_articleid;      // 文章ID
};

#endif