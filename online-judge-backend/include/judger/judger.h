#ifndef JUDGER_H
#define JUDGER_H

#include <json/json.h>

#include <string>

#include "constants/judge.h"

// 判题机
class Judger {
public:
    Judger();

    /**
     * 功能：判题函数
     * 传入数据：Json(SubmitId, ProblemId, JudgeNum, Code, Language, TimeLimit, MemoryLimit)
     * 传出数据：Json(Status, RunTime, RunMemory, Length, CompilerInfo, TestInfo(Status, StandardOutput, PersonalOutput,
     * RunTime, RunMemory))
     */
    Json::Value Run(Json::Value &runjson);

private:
    // 数据初始化
    bool Init(Json::Value &initjson);

    bool CompileSPJ();  // 编译 SPJ 文件

    bool GetCompilationFailed();  // 获取编译失败的原因

    // -----编译-----
    bool CompileC();  // 编译 C

    bool CompileCpp();  // 编译 C++

    bool CompileGo();  // 编译 Go

    bool CompileJava();  // 编译 Java

    bool CompilePython2();  // 编译 Python2

    bool CompilePython3();  // 编译 Python3

    bool CompileJavaScript();  // 编译 JavaScript

    // ------运行--------

    bool RunProgramC_Cpp();  // 运行 C 或者 C++

    bool RunProgramGo();  // 运行 Go

    bool RunProgramJava();  // 运行 Java

    bool RunProgramPython2();  // 运行 Python2

    bool RunProgramPython3();  // 运行 Python3

    bool RunProgramJavaScript();  // 运行 JavaScript

    bool RunProgram(struct config *conf);  // 运行程序

    void JudgmentResult(struct result *res, std::string &index);  // 判断结果

    Json::Value Done();  // 返回结果

private:
    Json::Value m_resjson;  // 存储运行结果的 Json

    std::string RUN_PATH;   // 运行的路径
    std::string DATA_PATH;  // 存储数据的路径
    bool m_isspj;           // 是否有 SPJ 文件

    std::string m_statusrecordid;  // 运行 ID
    std::string m_problemid;       // 题目 ID
    int m_judgenum;                // 测试用例数目
    std::string m_code;            // 代码

    int m_result;            // 运行结果
    std::string m_reason;    // 错误原因
    std::string m_command;   // 命令（中间变量）
    std::string m_language;  // 测评语言

    std::string m_length;  // 代码文件长度

    int m_timelimit;     // 时间限制
    long m_memorylimit;  // 空间限制

    int m_maxtimelimit;     // 最大时间限制
    long m_maxmemorylimie;  // 最大空间限制

    int m_runtime;     // 运行时间
    long m_runmemory;  // 运行空间
};

#endif
