#ifndef PROBLEMLIST_H
#define PROBLEMLIST_H

#include <map>
#include <string>
#include <jsoncpp/json/json.h>

// 题目类
class ProblemList
{
public:
    // 局部静态特性的方式实现单实例
    static ProblemList *GetInstance();

private:
    ProblemList();

    ~ProblemList();
};

#endif
