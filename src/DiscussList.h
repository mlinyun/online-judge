#ifndef DISCUSSLIST_H
#define DISCUSSLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 讨论类
class DiscussList {
public:
    // 局部静态特性的方式实现单实例
    static DiscussList *GetInstance();

private:
    DiscussList();

    ~DiscussList();
};

#endif