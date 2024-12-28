#include "StatusRecordList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>
#include <string>

using namespace std;

StatusRecordList *StatusRecordList::GetInstance()
{
    static StatusRecordList statusrecord;
    return &statusrecord;
}

// 分页查询测评记录
Json::Value StatusRecordList::SelectStatusRecordList(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectStatusRecordList(queryjson);
}

// 插入查询记录
string StatusRecordList::InsertStatusRecord(Json::Value &insertjson)
{
    return MoDB::GetInstance()->InsertStatusRecord(insertjson);
}

// 更新测评信息返回测评结果
bool StatusRecordList::UpdateStatusRecord(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateStatusRecord(updatejson);
}

// 查询一条详细测评记录
Json::Value StatusRecordList::SelectStatusRecord(Json::Value &queryjson)
{
    string statusrecordid = queryjson["SubmitId"].asString();

    // 获取缓存
    string resstr = ReDB::GetInstance()->GetStatusRecordCache(statusrecordid);

    Json::Value resjson;
    Json::Reader reader;
    // 如果有缓存
    if (resstr != "")
    {
        // 解析缓存json
        reader.parse(resstr, resjson);

        return resjson;
    }

    // 如果没有缓存
    resjson = MoDB::GetInstance()->SelectStatusRecord(queryjson);

    // 添加缓存 （状态不能为等待）
    if (resjson["Result"].asString() == "Success" && resjson["Status"].asInt() > 0)
    {
        ReDB::GetInstance()->AddStatusRecordCache(statusrecordid, resjson.toStyledString());
    }

    return resjson;
}

StatusRecordList::StatusRecordList()
{
}

StatusRecordList::~StatusRecordList()
{
}