#ifndef COMMENTLIST_H
#define COMMENTLIST_H

#include <jsoncpp/json/json.h>
#include <string>

// 评论类
class CommentList {
public:
    // 局部静态特性的方式实现单实例
    static CommentList *GetInstance();

    // 管理员查询评论
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    // 获取父评论
    Json::Value getFatherComment(Json::Value &queryjson);

    // 获取子评论
    Json::Value getSonComment(Json::Value &queryjson);

    // 插入父评论
    Json::Value InsertFatherComment(Json::Value &insertjson);

    // 插入子评论
    Json::Value InsertSonComment(Json::Value &insertjson);

    // 删除某一篇文章的所有评论
    bool DeleteArticleComment(Json::Value &deletejson);

    // 删除父评论
    Json::Value DeleteFatherComment(Json::Value &deletejson);

    // 删除子评论
    Json::Value DeleteSonComment(Json::Value &deletejson);

private:
    CommentList();

    ~CommentList();
};

#endif