#ifndef CONSTANTS_PROBLEM_H
#define CONSTANTS_PROBLEM_H

/**
 * 题目相关常量
 */
namespace constants {
namespace problem {
// problemdata 目录相对路径
constexpr const char* PROBLEMDATAPATH = "./problemdata/";

// 评测状态编号
constexpr int STATUS_COMPILE_ERROR = 1;
constexpr int STATUS_ACCEPTED = 2;
constexpr int STATUS_WRONG_ANSWER = 3;
constexpr int STATUS_RUNTIME_ERROR = 4;
constexpr int STATUS_TIME_LIMIT_EXCEEDED = 5;
constexpr int STATUS_MEMORY_LIMIT_EXCEEDED = 6;
constexpr int STATUS_SYSTEM_ERROR = 7;

// 各状态对应的题目统计字段
constexpr const char* FIELD_COMPILE_ERROR = "CENum";
constexpr const char* FIELD_ACCEPTED = "ACNum";
constexpr const char* FIELD_WRONG_ANSWER = "WANum";
constexpr const char* FIELD_RUNTIME_ERROR = "RENum";
constexpr const char* FIELD_TIME_LIMIT_EXCEEDED = "TLENum";
constexpr const char* FIELD_MEMORY_LIMIT_EXCEEDED = "MLENum";
constexpr const char* FIELD_SYSTEM_ERROR = "SENum";
}  // namespace problem
}  // namespace constants

#endif  // CONSTANTS_PROBLEM_H