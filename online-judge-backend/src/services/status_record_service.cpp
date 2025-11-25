#include "services/status_record_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
StatusRecordService *StatusRecordService::GetInstance() {
    static StatusRecordService status_record_service;
    return &status_record_service;
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

// 查询一条详细测评记录
Json::Value StatusRecordService::SelectStatusRecord(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectStatusRecord(queryjson);
}

StatusRecordService::StatusRecordService() {
    // 构造函数实现
}

StatusRecordService::~StatusRecordService() {
    // 析构函数实现
}