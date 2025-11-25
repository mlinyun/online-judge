#ifndef STATUS_RECORD_SERVICE_H
#define STATUS_RECORD_SERVICE_H

#include <json/json.h>

/**
 * 提交记录服务类头文件
 */
class StatusRecordService {
private:
    StatusRecordService();

    ~StatusRecordService();

public:
    // 局部静态特性的方式实现单实例模式
    static StatusRecordService *GetInstance();

    // 分页查询测评记录
    Json::Value SelectStatusRecordList(Json::Value &queryjson);

    // 插入查询记录
    std::string InsertStatusRecord(Json::Value &insertjson);

    // 更新测评信息返回测评结果
    bool UpdateStatusRecord(Json::Value &updatejson);

    // 查询一条详细测评记录
    Json::Value SelectStatusRecord(Json::Value &queryjson);
};

#endif  // STATUS_RECORD_SERVICE_H