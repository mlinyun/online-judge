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

Tag::Tag()
{
}

Tag::~Tag()
{
}