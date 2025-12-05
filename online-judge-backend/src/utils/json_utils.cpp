#include "utils/json_utils.h"

// 局部静态特性的方式实现单实例模式
JsonUtils *JsonUtils::GetInstance() {
    static JsonUtils json_utils;
    return &json_utils;
}

/**
 * 将 Json::Value 转换为字符串
 *
 * 解决 jsoncpp 默认会将中文编码成 \uXXXX 形式的问题
 *
 * @param jsonValue 要转换的 Json::Value 对象
 * @return 转换后的字符串表示
 */
std::string JsonUtils::JsonToString(const Json::Value &jsonValue) {
    Json::StreamWriterBuilder writerBuilder;
    // 配置 writerBuilder 以避免转义非 ASCII 字符
    writerBuilder["emitUTF8"] = true;   // 直接输出 UTF-8 字符
    writerBuilder["indentation"] = "";  // 可选：去除缩进，生成紧凑的 JSON 字符串，加快传输速度

    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    std::string resbody;
    std::ostringstream os;
    writer->write(jsonValue, &os);
    resbody = os.str();
    return resbody;
}

JsonUtils::JsonUtils(/* args */) {
    // 构造函数实现
}

JsonUtils::~JsonUtils() {
    // 析构函数实现
}