#ifndef TAG_H
#define TAG_H

#include <jsoncpp/json/json.h>

// 标签类
class Tag
{
public:
    // 局部静态特性的方式实现单实例
    static Tag *GetInstance();

private:
    Tag();

    ~Tag();
};

#endif