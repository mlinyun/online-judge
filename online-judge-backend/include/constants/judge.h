#ifndef CONSTANTS_JUDGE_H
#define CONSTANTS_JUDGE_H

/**
 * 判题相关常量
 */
namespace constants {
namespace judge {
// 判题状态
constexpr const char* STATUS_PENDING = "Pending";
constexpr const char* STATUS_RUNNING = "Running";
constexpr const char* STATUS_ACCEPTED = "Accepted";
constexpr const char* STATUS_WRONG_ANSWER = "Wrong Answer";
constexpr const char* STATUS_TIME_LIMIT_EXCEEDED = "Time Limit Exceeded";
constexpr const char* STATUS_MEMORY_LIMIT_EXCEEDED = "Memory Limit Exceeded";
constexpr const char* STATUS_RUNTIME_ERROR = "Runtime Error";
constexpr const char* STATUS_COMPILE_ERROR = "Compile Error";
constexpr const char* STATUS_SYSTEM_ERROR = "System Error";

// 资源限制
constexpr int DEFAULT_TIME_LIMIT_MS = 1000;   // 默认时间限制（毫秒）
constexpr int DEFAULT_MEMORY_LIMIT_MB = 256;  // 默认内存限制（MB）
constexpr int MAX_TIME_LIMIT_MS = 10000;      // 最大时间限制（毫秒）
constexpr int MAX_MEMORY_LIMIT_MB = 1024;     // 最大内存限制（MB）

// 编程语言
constexpr const char* LANG_CPP = "C++";
constexpr const char* LANG_C = "C";
constexpr const char* LANG_JAVA = "Java";
constexpr const char* LANG_PYTHON = "Python";
}  // namespace judge
}  // namespace constants

#endif  // CONSTANTS_JUDGE_H
