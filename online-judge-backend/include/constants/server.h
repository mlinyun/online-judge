#ifndef CONSTANTS_SERVER_H
#define CONSTANTS_SERVER_H

/**
 * 服务器相关常量
 */
namespace constants {
namespace server {
// HTTP 服务器配置
constexpr const char* HOST = "0.0.0.0";
constexpr int PORT = 8081;
constexpr int MAX_THREAD_COUNT = 8;
constexpr const char* API_PREFIX = "/api";

// 请求限制
constexpr int MAX_REQUEST_BODY_SIZE = 10 * 1024 * 1024;  // 10MB
constexpr int REQUEST_TIMEOUT_SECONDS = 30;

// 静态资源路径（相对于程序运行目录 online-judge-backend/）
constexpr const char* STATIC_ROOT = "./WWW";
constexpr const char* AVATAR_PATH = "./WWW/images";
}  // namespace server
}  // namespace constants

#endif  // CONSTANTS_SERVER_H
