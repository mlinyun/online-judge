#include "db/mongo_database.h"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/pipeline.hpp>

#include "constants/db.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace std;

/**
 * 功能：获取当前时间字符串
 * 返回值：时间字符串，格式为 "YYYY-MM-DD HH:MM:SS"
 */
string GetTime() {
    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    char str[50];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", now);
    return (string)str;
}

/**
 * 功能：获取指定集合的最大 ID
 * 参数：name 集合名称
 * 返回值：最大 ID
 */
int64_t MoDB::GetMaxId(string name) {
    auto client = pool.acquire();                                                      // 获取连接池中的客户端
    mongocxx::collection coll = (*client)[constants::db::DATABASE_NAME][name.data()];  // 访问指定集合

    bsoncxx::builder::stream::document document{};
    mongocxx::pipeline pipe;
    pipe.sort(make_document(kvp("id", -1))).limit(1);  // 构建管道：按 id 倒序取第一条
    mongocxx::cursor cursor = coll.aggregate(pipe);    // 执行聚合查询

    // 如果游标为空，说明集合没有数据，返回 0
    if (cursor.begin() == cursor.end()) {
        return 0;
    }

    Json::Value jsonvalue;
    Json::Reader reader;
    for (auto doc : cursor) {
        reader.parse(bsoncxx::to_json(doc), jsonvalue);  // 将 BSON 转为 JSON 以便读取字段
    }

    int64_t max_id = stoll(jsonvalue["_id"].asString());  // 读取 _id 字段并转为 int64
    return max_id;
}

// 局部静态特性的方式实现单实例模式
MoDB *MoDB::GetInstance() {
    static MoDB modb;
    return &modb;
}

MoDB::MoDB() {
    // 构造函数实现

    // 初始化 ID
    m_problem_id = GetMaxId(constants::db::COLLECTION_PROBLEMS);
    m_status_record_id = GetMaxId(constants::db::COLLECTION_STATUS_RECORDS);
    m_comment_id = GetMaxId(constants::db::COLLECTION_COMMENTS);
    int64_t m_announcement_id = GetMaxId(constants::db::COLLECTION_ANNOUNCEMENTS);
    int64_t m_solution_id = GetMaxId(constants::db::COLLECTION_SOLUTIONS);
    int64_t m_discussion_id = GetMaxId(constants::db::COLLECTION_DISCUSSIONS);
    m_article_id = max(m_solution_id, max(m_discussion_id, m_announcement_id));
}

MoDB::~MoDB() {
    // 析构函数实现
}