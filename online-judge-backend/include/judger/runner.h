#ifndef JUDGER_RUNNER_H
#define JUDGER_RUNNER_H

#include <stdio.h>
#include <sys/types.h>

// (ver >> 16) & 0xff, (ver >> 8) & 0xff, ver & 0xff  -> real version
#define VERSION 0x020101

#define UNLIMITED -1

#define LOG_ERROR(error_code) LOG_FATAL(log_fp, "Error: " #error_code);

#define ERROR_EXIT(error_code)       \
    {                                \
        LOG_ERROR(error_code);       \
        _result->error = error_code; \
        log_close(log_fp);           \
        return;                      \
    }

#define ARGS_MAX_NUMBER 256
#define ENV_MAX_NUMBER 256

// 青岛大学的开源安全沙箱文档地址：https://opensource.qduoj.com/#/judger/api

// error 返回值
enum {
    SUCCESS = 0,
    INVALID_CONFIG = -1,
    FORK_FAILED = -2,
    PTHREAD_FAILED = -3,
    WAIT_FAILED = -4,
    ROOT_REQUIRED = -5,
    LOAD_SECCOMP_FAILED = -6,
    SETRLIMIT_FAILED = -7,
    DUP2_FAILED = -8,
    SETUID_FAILED = -9,
    EXECVE_FAILED = -10,
    SPJ_ERROR = -11
};

// result 返回值
enum {
    WRONG_ANSWER = -1,
    CPU_TIME_LIMIT_EXCEEDED = 1,
    REAL_TIME_LIMIT_EXCEEDED = 2,
    MEMORY_LIMIT_EXCEEDED = 3,
    RUNTIME_ERROR = 4,
    SYSTEM_ERROR = 5
};

struct config {
    int max_cpu_time;        // 设置进程可占用的最大 CPU 时间，-1 表示无限制
    int max_real_time;       // 设置进程可运行的最大时间，-1 表示无限制运行
    long max_memory;         // 设置进程虚拟内存（地址空间）的最大大小，-1 表示无限制
    long max_stack;          // 设置进程堆栈的最大大小
    int max_process_number;  // 设置调用进程的实际用户 ID 可以创建的最大进程数，-1 表示无限制
    long max_output_size;  // 设置进程可以输出到标准输出、标准错误输出和文件的最大数据大小，-1 表示无限制输出
    int memory_limit_check_only;  // 如果此值等于 0 ，则我们只会检查内存使用量，因为 setrlimit(maxrss)
                                  // 会导致一些崩溃问题
    char *exe_path;     // 设置要运行的可执行文件路径
    char *input_path;   // 设置标准输入重定向的文件路径
    char *output_path;  // 设置标准输出重定向的文件路径
    char *error_path;   // 设置标准错误输出重定向的文件路径
    char *args[ARGS_MAX_NUMBER];
    char *env[ENV_MAX_NUMBER];
    char *log_path;           // 设置判断日志文件路径
    char *seccomp_rule_name;  // 用于限制进程系统调用的 seccomp 规则。名称用于调用相应的函数。
    uid_t uid;                // 运行此进程的用户 ID
    gid_t gid;                // 运行此进程的组 ID
};

struct result {
    int cpu_time;   // 进程已使用的 CPU 时间
    int real_time;  // 进程的实际运行时间
    long memory;    // 进程使用的最大内存值
    int signal;     // 信号编号
    int exit_code;  // 进程的退出代码
    int error;      // 参数验证错误或判断器内部错误，详情请参见上方枚举
    int result;     // 评判结果，详情请参见上方枚举
};

void run(struct config *, struct result *);
#endif  // JUDGER_RUNNER_H
