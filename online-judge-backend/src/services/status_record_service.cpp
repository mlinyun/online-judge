#include "services/status_record_service.h"

#include <utils/json_utils.h>

#include "db/mongo_database.h"
#include "db/redis_database.h"
#include "utils/response.h"

// 局部静态特性的方式实现单实例模式
StatusRecordService *StatusRecordService::GetInstance() {
    static StatusRecordService status_record_service;
    return &status_record_service;
}

// 查询一条详细测评记录
Json::Value StatusRecordService::SelectStatusRecord(Json::Value &queryjson) {
    // 获取提交 ID
    string statusrecordid = queryjson["StatusRecordId"].asString();
    // 获取缓存
    string status_record_cache = ReDB::GetInstance()->GetStatusRecordCache(statusrecordid);
    Json::Value resjson;
    Json::Reader reader;
    // 如果有缓存
    if (status_record_cache != "") {
        // 解析缓存 Json
        reader.parse(status_record_cache, resjson);
        return resjson;
    }
    // 如果没有缓存
    resjson = MoDB::GetInstance()->SelectStatusRecord(queryjson);
    // 添加缓存（状态不能为等待）
    if (resjson["success"].asBool() && resjson["data"]["Status"].asInt() > 0) {
        string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
        ReDB::GetInstance()->AddStatusRecordCache(statusrecordid, resbody);
    }
    return resjson;
}

// 分页查询测评记录
Json::Value StatusRecordService::SelectStatusRecordList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectStatusRecordList(queryjson);
}

// 插入查询记录
std::string StatusRecordService::InsertStatusRecord(Json::Value &insertjson) {
    return MoDB::GetInstance()->InsertStatusRecord(insertjson);
}

// 更新测评信息返回测评结果
bool StatusRecordService::UpdateStatusRecord(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateStatusRecord(updatejson);
}

// 获取状态记录的作者 UserId
std::string StatusRecordService::GetStatusRecordAuthorId(int64_t statusRecordId) {
    return MoDB::GetInstance()->GetStatusRecordAuthorId(statusRecordId);
}

StatusRecordService::StatusRecordService() {
    // 构造函数实现
}

StatusRecordService::~StatusRecordService() {
    // 析构函数实现
}