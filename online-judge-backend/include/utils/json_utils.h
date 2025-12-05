#ifndef JSON_UTILS_H
#define JSON_UTILS_H
#include <json/json.h>

#include <string>

class JsonUtils {
private:
    JsonUtils();

    ~JsonUtils();

public:
    // 局部静态特性的方式实现单实例模式
    static JsonUtils *GetInstance();

    // 将 Json::Value 转换为字符串
    std::string JsonToString(const Json::Value &jsonValue);
};

#endif  // JSON_UTILS_H