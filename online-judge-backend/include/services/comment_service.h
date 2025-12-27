#ifndef COMMENT_SERVICE_H
#define COMMENT_SERVICE_H

#include <json/json.h>

/**
 * 评论服务类头文件
 */
class CommentService {
private:
    CommentService();

    ~CommentService();

public:
    // 局部静态特性的方式实现单实例模式
    static CommentService *GetInstance();
    // 插入父评论
    Json::Value InsertFatherComment(Json::Value &insertjson);

    // 插入子评论
    Json::Value InsertSonComment(Json::Value &insertjson);

    // 获取父评论
    Json::Value GetFatherComment(Json::Value &queryjson);

    // 获取子评论
    Json::Value GetSonComment(Json::Value &queryjson);

    // 管理员查询评论
    Json::Value SelectCommentListByAdmin(Json::Value &queryjson);

    // 评论点赞/取消点赞（单接口 Toggle）
    Json::Value ToggleCommentLike(Json::Value &likejson);

    // 删除父评论
    Json::Value DeleteFatherComment(Json::Value &deletejson);

    // 删除子评论
    Json::Value DeleteSonComment(Json::Value &deletejson);

    // 删除某一篇文章的所有评论
    bool DeleteArticleComment(Json::Value &deletejson);

    /**
     * 功能：获取评论的作者 UserId
     * 传入：评论 ID
     * 传出：作者 UserId，如果不存在返回空字符串
     */
    std::string GetCommentAuthorId(int64_t commentId);
};

#endif  // COMMENT_SERVICE_H