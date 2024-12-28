#include "CommentList.h"
#include "MongoDataBase.h"

CommentList *CommentList::GetInstance()
{
    static CommentList commentlist;
    return &commentlist;
}

// 管理员查询评论
Json::Value CommentList::SelectCommentListByAdmin(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectCommentListByAdmin(queryjson);
}

// 获取父评论
Json::Value CommentList::getFatherComment(Json::Value &queryjson)
{
    return MoDB::GetInstance()->getFatherComment(queryjson);
}

// 获取子评论
Json::Value CommentList::getSonComment(Json::Value &queryjson)
{
    return MoDB::GetInstance()->getSonComment(queryjson);
}

// 插入父评论
Json::Value CommentList::InsertFatherComment(Json::Value &insertjson)
{
    return MoDB::GetInstance()->InsertFatherComment(insertjson);
}

// 插入子评论
Json::Value CommentList::InsertSonComment(Json::Value &insertjson)
{
    return MoDB::GetInstance()->InsertSonComment(insertjson);
}

// 删除某一篇文章的所有评论
bool CommentList::DeleteArticleComment(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteArticleComment(deletejson);
}

// 删除父评论
Json::Value CommentList::DeleteFatherComment(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteFatherComment(deletejson);
}

// 删除子评论
Json::Value CommentList::DeleteSonComment(Json::Value &deletejson)
{
    return MoDB::GetInstance()->DeleteSonComment(deletejson);
}

CommentList::CommentList()
{
}

CommentList::~CommentList()
{
}