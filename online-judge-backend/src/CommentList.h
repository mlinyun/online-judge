#ifndef COMMENTLIST_H
#define COMMENTLIST_H
#include <jsoncpp/json/json.h>
#include <string>

// 评论类
class CommentList
{
public:
    // 局部静态特性的方式实现单实例
    static CommentList *GetInstance();

private:
    CommentList();

    ~CommentList();
};

#endif