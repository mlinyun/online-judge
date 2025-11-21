#ifndef JUDGER_H
#define JUDGER_H

#include <jsoncpp/json/json.h>
#include <string>

enum Status
{
    PJ,  // PJ "Pending & Judging"
    CE,  // CE "Compile Error"
    AC,  // AC "Accepted"
    WA,  // WA "Wrong Answer"
    RE,  // RE "Runtime Error"
    TLE, // TLE "Time Limit Exceeded"
    MLE, // MLE "Memory Limit Exceeded"
    SE   // SE "System Error"
};

// 判题机
class Judger
{
public:
    Judger();

    /*
        功能：判题函数
        传入数据：Json(SubmitId, ProblemId, JudgeNum, Code, Language, TimeLimit, MemoryLimit)
        传出数据：Json(Status, RunTime, RunMemory, Length, CompilerInfo,
            TestInfo(Status, StandardOutput, PersonalOutput, RunTime, RunMemory))
    */
    Json::Value Run(Json::Value &runjson);

private:
    // 数据初始化
    bool Init(Json::Value &initjson);

    bool CompileSPJ(); // 编译SPJ文件

    bool GetCompilationFailed(); // 获取编译失败的原因

    // -----编译-----
    bool CompileC(); // 编译 C

    bool CompileCpp(); // 编译 C++

    bool CompileGo(); // 编译 Go

    bool CompileJava(); // 编译 Java

    bool CompilePython2(); // 编译 Python2

    bool CompilePython3(); // 编译 Python3

    bool CompileJavaScript(); // 编译 JavaScript

    // ------运行--------

    bool RunProgramC_Cpp(); // 运行 C 或者 C++

    bool RunProgramGo(); // 运行 Go

    bool RunProgramJava(); // 运行 Java

    bool RunProgramPython2(); // 运行 Python2

    bool RunProgramPython3(); // 运行 Python3

    bool RunProgramJavaScript(); // 运行 JavaScript

    bool RunProgram(struct config *conf); // 运行程序

    bool JudgmentResult(struct result *res, std::string &index); // 判断结果

    Json::Value Done(); // 返回结果

private:
    Json::Value m_resjson; // 存储运行结果的Json

    std::string RUN_PATH;  // 运行的路径
    std::string DATA_PATH; // 存储数据的路径
    bool m_isspj;          // 是否有SPJ文件

    std::string m_submitid;  // 运行ID
    std::string m_problemid; // 题目ID
    int m_judgenum;          // 测试用例数目
    std::string m_code;      // 代码

    int m_result;           // 运行结果
    std::string m_reason;   // 错误原因
    std::string m_command;  // 命令（中间变量）
    std::string m_language; // 测评语言

    std::string m_length; // 文件长度

    int m_timelimit;    // 时间限制
    long m_memorylimit; // 空间限制

    int m_maxtimelimit;    // 最大时间限制
    long m_maxmemorylimie; // 最大空间限制

    int m_runtime;    // 运行时间
    long m_runmemory; // 运行空间
};

#endif
