#include "MongoDataBase.h"
#include <cstdint>
#include <iostream>
#include <ctime>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/pipeline.hpp>

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace std;

// 获取时间
string GetTime() {
    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    char str[50];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", now);
    return (string) str;
}

MoDB *MoDB::GetInstance() {
    static MoDB modb;
    return &modb;
}

/*
    功能：获取某一个集合中最大的ID
*/
int64_t MoDB::GetMaxId(std::string name) {
    auto client = pool.acquire();
    mongocxx::collection coll = (*client)["XDOJ"][name.data()];

    bsoncxx::builder::stream::document document{};
    mongocxx::pipeline pipe;
    pipe.sort({make_document(kvp("_id", -1))});
    pipe.limit(1);
    mongocxx::cursor cursor = coll.aggregate(pipe);

    // 如果没找到，说明集合中没有数据
    if (cursor.begin() == cursor.end())
        return 0;

    Json::Value jsonvalue;
    Json::Reader reader;
    for (auto doc: cursor) {
        reader.parse(bsoncxx::to_json(doc), jsonvalue);
    }

    int64_t id = stoll(jsonvalue["_id"].asString());
    return id;
}

MoDB::MoDB() {
    // 初始化ID
    m_problemid = GetMaxId("Problem");
    m_statusrecordid = GetMaxId("StatusRecord");
    m_commentid = GetMaxId("Comment");
    int64_t m_announcementid = GetMaxId("Announcement");
    int64_t m_solutionid = GetMaxId("Solution");
    int64_t m_discussid = GetMaxId("Discuss");
    m_articleid = max(m_solutionid, max(m_discussid, m_announcementid));
}

MoDB::~MoDB() {
}