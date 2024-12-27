#include "Tag.h"
#include "MongoDataBase.h"
#include <iostream>

using namespace std;

// 局部静态特性的方式实现单实例
Tag *Tag::GetInstance()
{
    static Tag tag;
    return &tag;
}

// 初始化题目标签
void Tag::InitProblemTags()
{
    Json::Value jsonvalue = MoDB::GetInstance()->getProblemTags();
    problemtags["Tags"] = jsonvalue["values"];
}

// 获取题目的所有标签
Json::Value Tag::getProblemTags()
{
    return problemtags;
}

Tag::Tag()
{
}

Tag::~Tag()
{
}