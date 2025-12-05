#ifndef CONSTANTS_JUDGE_H
#define CONSTANTS_JUDGE_H

/**
 * 判题相关常量
 */

// 判题状态枚举
enum Status {
    PJ,   // PJ "Pending & Judging"
    CE,   // CE "Compile Error"
    AC,   // AC "Accepted"
    WA,   // WA "Wrong Answer"
    RE,   // RE "Runtime Error"
    TLE,  // TLE "Time Limit Exceeded"
    MLE,  // MLE "Memory Limit Exceeded"
    SE    // SE "System Error"
};

namespace constants {
namespace judge {
// 评测状态编号
constexpr int STATUS_PENDING_JUDGING = 0;      // 等待与判题中 "PJ"
constexpr int STATUS_COMPILE_ERROR = 1;          // 编译错误 "CENum"
constexpr int STATUS_ACCEPTED = 2;               // 成功 "ACNum"
constexpr int STATUS_WRONG_ANSWER = 3;           // 错误答案 "WANum"
constexpr int STATUS_RUNTIME_ERROR = 4;          // 运行时错误 "RENum"
constexpr int STATUS_TIME_LIMIT_EXCEEDED = 5;    // 超时 "TLENum"
constexpr int STATUS_MEMORY_LIMIT_EXCEEDED = 6;  // 内存超限 "MLENum"
constexpr int STATUS_SYSTEM_ERROR = 7;           // 系统错误 "SENum"

// 各状态对应的题目统计字段
constexpr const char* FIELD_COMPILE_ERROR = "CENum";           // 编译错误 1
constexpr const char* FIELD_ACCEPTED = "ACNum";                // 成功 2
constexpr const char* FIELD_WRONG_ANSWER = "WANum";            // 错误答案 3
constexpr const char* FIELD_RUNTIME_ERROR = "RENum";           // 运行时错误 4
constexpr const char* FIELD_TIME_LIMIT_EXCEEDED = "TLENum";    // 超时 5
constexpr const char* FIELD_MEMORY_LIMIT_EXCEEDED = "MLENum";  // 内存超限 6
constexpr const char* FIELD_SYSTEM_ERROR = "SENum";            // 系统错误 7

// 资源限制
constexpr int DEFAULT_TIME_LIMIT_MS = 1000;   // 默认时间限制（毫秒）
constexpr int DEFAULT_MEMORY_LIMIT_MB = 256;  // 默认内存限制（MB）
constexpr int MAX_TIME_LIMIT_MS = 10000;      // 最大时间限制（毫秒）
constexpr int MAX_MEMORY_LIMIT_MB = 1024;     // 最大内存限制（MB）

// 代码运行的路径
constexpr const char* RUN_PATH_PREFIX = "./tmp/";
// 存储题目数据的路径
constexpr const char* PROBLEM_DATA_PREFIX = "./problemdata/";

// 编程语言
constexpr const char* LANG_C = "C";
constexpr const char* LANG_CPP = "C++";
constexpr const char* LANG_GO = "Go";
constexpr const char* LANG_JAVA = "Java";
constexpr const char* LANG_PYTHON2 = "Python2";
constexpr const char* LANG_PYTHON3 = "Python3";
constexpr const char* LANG_JAVASCRIPT = "JavaScript";
}  // namespace judge
}  // namespace constants

#endif  // CONSTANTS_JUDGE_H
