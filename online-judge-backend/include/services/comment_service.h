#ifndef COMMENT_SERVICE_H
#define COMMENT_SERVICE_H

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
};

#endif  // COMMENT_SERVICE_H