#ifndef TAG_H
#define TAG_H

#include <jsoncpp/json/json.h>

// 标签类
class Tag
{
public:
    // 局部静态特性的方式实现单实例
    static Tag *GetInstance();
    // 初始化题目标签
    void InitProblemTags();
    // 获取题目的所有标签
    Json::Value getProblemTags();

private:
    Json::Value problemtags;

    Tag();

    ~Tag();
};

#endif