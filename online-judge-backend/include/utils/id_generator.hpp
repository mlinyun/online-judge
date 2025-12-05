#pragma once
#include <mutex>

#include "snowflake.hpp"

/**
 * 唯一 ID 生成器（基于雪花算法）
 * 全局单例封装
 */
class IDGenerator {
public:
    using snowflake_t = snowflake<1534832906275L, std::mutex>;

    static IDGenerator& Instance() {
        static IDGenerator instance;
        return instance;
    }

    // 初始化（只调用一次）
    void Init(int64_t workerid, int64_t datacenterid) { generator_.init(workerid, datacenterid); }

    // 生成唯一 ID
    int64_t NextId() { return generator_.nextid(); }

private:
    IDGenerator() = default;
    ~IDGenerator() = default;

    IDGenerator(const IDGenerator&) = delete;
    IDGenerator& operator=(const IDGenerator&) = delete;

private:
    snowflake_t generator_;
};
