#include "services/tag_service.h"

#include "db/mongo_database.h"
#include "utils/response.h"

// 局部静态特性的方式实现单实例模式
TagService *TagService::GetInstance() {
    static TagService tag_service;
    return &tag_service;
}

// 初始化题目标签（在编辑题目时会调用）
void TagService::InitProblemTags() {
    Json::Value tags = MoDB::GetInstance()->GetProblemTags();
    problem_tags["Tags"] = tags["values"];
}

// 获取题目的所有标签
Json::Value TagService::GetProblemTags() {
    return response::Success("获取题目标签成功", problem_tags);
}

TagService::TagService() {
    // 构造函数实现
}

TagService::~TagService() {
    // 析构函数实现
}