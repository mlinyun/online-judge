#ifndef TAG_SERVICE_H
#define TAG_SERVICE_H

#include <json/json.h>

/**
 * 标签服务类头文件
 */
class TagService {
private:
    Json::Value problem_tags;  // 题目标签

    TagService();

    ~TagService();

public:
    // 局部静态特性的方式实现单实例模式
    static TagService *GetInstance();

    // 初始化题目标签
    void InitProblemTags();

    // 获取题目的所有标签
    Json::Value GetProblemTags();
};

#endif  // TAG_SERVICE_H