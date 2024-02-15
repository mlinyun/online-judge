#ifndef SOLUTIONLIST_H
#define SOLUTIONLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 讨论类
class SolutionList {
public:
    // 局部静态特性的方式实现单实例
    static SolutionList *GetInstance();

private:
    SolutionList();

    ~SolutionList();
};

#endif