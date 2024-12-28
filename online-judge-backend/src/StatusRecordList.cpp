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
    return MoDB::GetInstance()->SelectStatusRecord(queryjson);
}

StatusRecordList::StatusRecordList()
{
}

StatusRecordList::~StatusRecordList()
{
}