#ifndef STATUSRECORDLIST_H
#define STATUSRECORDLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 状态记录类，保存代码判定结果的类
class StatusRecordList {
public:
    // 局部静态特性的方式实现单实例
    static StatusRecordList *GetInstance();

private:
    StatusRecordList();

    ~StatusRecordList();
};

#endif