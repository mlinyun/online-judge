#ifndef DISCUSS_SERVICE_H
#define DISCUSS_SERVICE_H

#include <json/json.h>

/**
 * 讨论服务类头文件
 */
class DiscussService {
private:
    DiscussService();

    ~DiscussService();

public:
    // 局部静态特性的方式实现单实例模式
    static DiscussService *GetInstance();

    // 添加讨论
    Json::Value InsertDiscuss(Json::Value &insertjson);

    // 查询讨论的详细内容，并且将其浏览量加 1
    Json::Value SelectDiscuss(Json::Value &queryjson);

    // 更新讨论
    Json::Value UpdateDiscuss(Json::Value &updatejson);

    // 删除讨论
    Json::Value DeleteDiscuss(Json::Value &deletejson);

    // 分页查询讨论
    Json::Value SelectDiscussList(Json::Value &queryjson);

    // 分页查询讨论（管理员权限）
    Json::Value SelectDiscussListByAdmin(Json::Value &queryjson);

    // 查询讨论的详细信息，主要是编辑时的查询
    Json::Value SelectDiscussByEdit(Json::Value &queryjson);

    // 更新讨论的评论数量
    bool UpdateDiscussComments(Json::Value &updatejson);
};

#endif  // DISCUSS_SERVICE_H