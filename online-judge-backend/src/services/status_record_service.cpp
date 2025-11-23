#include "services/status_record_service.h"

#include "db/mongo_database.h"

// 局部静态特性的方式实现单实例模式
StatusRecordService* StatusRecordService::GetInstance() {
    static StatusRecordService status_record_service;
    return &status_record_service;
}

StatusRecordService::StatusRecordService() {
    // 构造函数实现
}

StatusRecordService::~StatusRecordService() {
    // 析构函数实现
}