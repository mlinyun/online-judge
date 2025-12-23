#include "judger/judger.h"

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "constants/judge.h"

extern "C" {
#include "judger/runner.h"
}

using namespace std;

// 获取文件大小
size_t GetFileSize(const char *fileName) {
    if (fileName == NULL) {
        return 0;
    }
    struct stat statbuf;

    stat(fileName, &statbuf);

    size_t filesize = statbuf.st_size;

    return filesize;
}

Judger::Judger() {}

Json::Value Judger::Run(Json::Value &runjson) {
    // 初始化数据
    if (!Init(runjson)) {
        return Done();
    }

    // 编译 运行
    if (m_language == constants::judge::LANG_C) {
        if (!CompileC()) {
            return Done();
        }

        if (!RunProgramC_Cpp()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_CPP) {
        if (!CompileCpp()) {
            return Done();
        }

        if (!RunProgramC_Cpp()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_GO) {
        if (!CompileGo()) {
            return Done();
        }

        if (!RunProgramGo()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_JAVA) {
        if (!CompileJava()) {
            return Done();
        }

        if (!RunProgramJava()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_PYTHON2) {
        if (!CompilePython2()) {
            return Done();
        }

        if (!RunProgramPython2()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_PYTHON3) {
        if (!CompilePython3()) {
            return Done();
        }

        if (!RunProgramPython3()) {
            return Done();
        }
    } else if (m_language == constants::judge::LANG_JAVASCRIPT) {
        if (!CompileJavaScript()) {
            return Done();
        }

        if (!RunProgramJavaScript()) {
            return Done();
        }
    }

    return Done();
}

/**
 * 功能：数据初始化
 */
bool Judger::Init(Json::Value &initjson) {
    // 初始化数据
    // Json(StatusRecordId, ProblemId, JudgeNum, Code, Language, TimeLimit, MemoryLimit)
    m_statusrecordid = initjson["StatusRecordId"].asString();
    m_problemid = initjson["ProblemId"].asString();
    m_code = initjson["Code"].asString();

    m_judgenum = initjson["JudgeNum"].asInt();
    m_timelimit = initjson["TimeLimit"].asInt();
    m_memorylimit = initjson["MemoryLimit"].asLargestInt() * 1024 * 1024;
    m_language = initjson["Language"].asString();
    m_maxtimelimit = m_timelimit * 2;
    m_maxmemorylimie = m_memorylimit * 2;
    m_result = PJ;
    m_reason = "";
    m_runmemory = 0;
    m_runtime = 0;
    m_isspj = false;

    RUN_PATH = constants::judge::RUN_PATH_PREFIX + m_statusrecordid + "/";
    DATA_PATH = constants::judge::PROBLEM_DATA_PREFIX + m_problemid + "/";

    m_resjson.clear();

    // 创建中间文件夹
    m_command = "mkdir -p " + RUN_PATH;
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    // 将代码输出到文件中
    ofstream outfile;
    if (m_language == constants::judge::LANG_C) {
        m_command = RUN_PATH + "main.c";
    } else if (m_language == constants::judge::LANG_CPP) {
        m_command = RUN_PATH + "main.cpp";
    } else if (m_language == constants::judge::LANG_GO) {
        m_command = RUN_PATH + "main.go";
    } else if (m_language == constants::judge::LANG_JAVA) {
        m_command = RUN_PATH + "Main.java";
    } else if (m_language == constants::judge::LANG_PYTHON2) {
        m_command = RUN_PATH + "main.py";
    } else if (m_language == constants::judge::LANG_PYTHON3) {
        m_command = RUN_PATH + "main.py";
    } else if (m_language == constants::judge::LANG_JAVASCRIPT) {
        m_command = RUN_PATH + "main.js";
    } else {
        m_result = SE;
        return false;
    }
    outfile.open(m_command.data());
    if (!outfile.is_open()) {
        m_result = SE;
        return false;
    }
    outfile << m_code.data();
    outfile.close();

    m_length = to_string(GetFileSize(m_command.data())) + "B";

    // 编译 spj 文件
    if (!CompileSPJ()) {
        m_result = SE;
        return false;
    }

    return true;
}

bool Judger::CompileSPJ() {
    // 如果存在可执行文件 spj
    m_command = DATA_PATH + "spj";
    if (access(m_command.data(), F_OK) == 0) {
        m_isspj = true;
        return true;
    }

    // 如果不存在 spj.cpp 文件
    m_command = DATA_PATH + "spj.cpp";
    if (access(m_command.data(), F_OK) == -1) {
        m_isspj = false;
        return true;
    }

    // 编译 SPJ 文件
    m_command = "timeout 10 g++ " + DATA_PATH + "spj.cpp -o " + DATA_PATH + "spj -O2 -std=c++17";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }
    return true;
}

bool Judger::GetCompilationFailed() {
    ifstream infile;

    // 只要编译就会有这个文件
    m_command = RUN_PATH + "compileinfo.txt";
    infile.open(m_command.data());
    // 读取全部信息
    string reason((istreambuf_iterator<char>(infile)), (istreambuf_iterator<char>()));
    m_reason = reason;
    m_result = CE;
    infile.close();

    return true;
}

// 编译 C 函数
bool Judger::CompileC() {
    // 进行gcc编译
    m_command = "timeout 10 gcc " + RUN_PATH + "main.c -fmax-errors=3 -o " + RUN_PATH + "main -O2 -std=c11 2>" +
                RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "main";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 编译 C++ 函数
bool Judger::CompileCpp() {
    // 进行g++编译
    m_command = "timeout 10 g++ " + RUN_PATH + "main.cpp -fmax-errors=3 -o " + RUN_PATH + "main -O2 -std=c++11 2>" +
                RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "main";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 编译 Go 函数
bool Judger::CompileGo() {
    // 进行go编译
    m_command = "go build -o " + RUN_PATH + "main " + RUN_PATH + "main.go 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "main";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 编译 Java 函数
bool Judger::CompileJava() {
    // 创建目标目录
    std::string outputDir = RUN_PATH + "Main";
    if (access(outputDir.data(), F_OK) == -1) {
        if (system(("mkdir -p " + outputDir).c_str()) == -1) {
            m_result = SE;
            return false;
        }
    }

    // 进行 java 编译
    m_command = "javac " + RUN_PATH + "Main.java -d " + outputDir + " 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    // 检查 Main.class 文件是否存在
    m_command = outputDir + "/Main.class";
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }

    return true;
}

// 编译 Python2 函数
bool Judger::CompilePython2() {
    // 进行 Python2 编译
    m_command = "python2 -m py_compile " + RUN_PATH + "main.py 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "main.pyc";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 编译 Python3 函数
bool Judger::CompilePython3() {
    // 进行 Python3 编译
    m_command = "python3 -m py_compile " + RUN_PATH + "main.py 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "__pycache__/main.cpython-38.pyc";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 编译 JavaScript 函数
bool Judger::CompileJavaScript() {
    // 进行 JavaScript 编译
    m_command = "/usr/bin/nodejs --check " + RUN_PATH + "main.js 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    // TODO:不能返回错误，因为肯定有这个文件
    m_command = RUN_PATH + "main.js";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

// 运行 C 或者 C++ 函数
bool Judger::RunProgramC_Cpp() {
    // 创建配置结构体
    struct config conf = {};

    conf.max_cpu_time = m_timelimit;
    conf.max_real_time = m_maxtimelimit;
    conf.max_memory = m_maxmemorylimie;
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 0;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = (char *)"c_cpp";

    string exe_path = RUN_PATH + "main";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    RunProgram(&conf);
    return true;
}

// 运行 Go 函数
bool Judger::RunProgramGo() {
    // 创建配置结构体
    struct config conf = {};

    conf.max_cpu_time = m_timelimit;
    conf.max_real_time = m_maxtimelimit;
    conf.max_memory = m_maxmemorylimie;
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = (char *)"golang";

    string exe_path = RUN_PATH + "main";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    conf.env[0] = (char *)"LANG=en_US.UTF-8";
    conf.env[1] = (char *)"LANGUAGE=en_US:en";
    conf.env[2] = (char *)"LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

// 运行 Java 函数
bool Judger::RunProgramJava() {
    // 创建配置结构体
    struct config conf = {};
    m_memorylimit = m_memorylimit * 3;  // Java 的空间限制为原来的三倍

    conf.max_cpu_time = m_timelimit * 3;
    conf.max_real_time = m_maxtimelimit * 3;
    conf.max_memory = -1;  // Java 不能限制内存，在虚拟机中限制
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = nullptr;

    string exe_path = "/usr/bin/java";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    string exe_file = RUN_PATH + "Main";
    string tmp_maxmemory = "-XX:MaxRAM=" + to_string(m_memorylimit) + "k";

    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    conf.args[0] = (char *)"/usr/bin/java";
    conf.args[1] = (char *)"-cp";
    conf.args[2] = (char *)exe_file.data();
    conf.args[3] = (char *)tmp_maxmemory.data();
    conf.args[4] = (char *)"-Djava.security.policy==policy";
    conf.args[5] = (char *)"-Djava.awt.headless=true";
    conf.args[6] = (char *)"Main";

    conf.env[0] = (char *)"LANG=en_US.UTF-8";
    conf.env[1] = (char *)"LANGUAGE=en_US:en";
    conf.env[2] = (char *)"LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

// 运行 Python2 函数
bool Judger::RunProgramPython2() {
    // 创建配置结构体
    struct config conf = {};

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 0;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = (char *)"general";

    string exe_path = "/usr/bin/python2";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    string exe_file = RUN_PATH + "main.pyc";

    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    conf.args[0] = (char *)"/usr/bin/python2";
    conf.args[1] = (char *)exe_file.data();

    conf.env[0] = (char *)"LANG=en_US.UTF-8";
    conf.env[1] = (char *)"LANGUAGE=en_US:en";
    conf.env[2] = (char *)"LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

// 运行 Python3 函数
bool Judger::RunProgramPython3() {
    // 创建配置结构体
    struct config conf = {};

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 0;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = (char *)"general";

    string exe_path = "/usr/bin/python3";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    string exe_file = RUN_PATH + "__pycache__/main.cpython-38.pyc";

    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    conf.args[0] = (char *)"/usr/bin/python3";
    conf.args[1] = (char *)exe_file.data();

    conf.env[0] = (char *)"LANG=en_US.UTF-8";
    conf.env[1] = (char *)"LANGUAGE=en_US:en";
    conf.env[2] = (char *)"LC_ALL=en_US.UTF-8";
    conf.env[3] = (char *)"PYTHONIOENCODING=utf-8";

    RunProgram(&conf);
    return true;
}

// 运行 JavaScript 函数
bool Judger::RunProgramJavaScript() {
    // 创建配置结构体
    struct config conf = {};

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;
    conf.max_stack = 32 * 1024 * 1024;
    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    conf.seccomp_rule_name = (char *)"node";

    string exe_path = "/usr/bin/nodejs";
    string error_path = RUN_PATH + "error.out";
    string log_path = RUN_PATH + "judger.log";
    string exe_file = RUN_PATH + "main.js";

    conf.exe_path = (char *)exe_path.data();
    conf.error_path = (char *)error_path.data();
    conf.log_path = (char *)log_path.data();

    conf.args[0] = (char *)"/usr/bin/nodejs";
    conf.args[1] = (char *)exe_file.data();

    conf.env[0] = (char *)"LANG=en_US.UTF-8";
    conf.env[1] = (char *)"LANGUAGE=en_US:en";
    conf.env[2] = (char *)"LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

// 运行程序并判定所有测试用例
bool Judger::RunProgram(struct config *conf) {
    struct result res = {};

    for (int i = 1; i <= m_judgenum; i++) {
        string index = to_string(i);
        string input_path = DATA_PATH + index + ".in";
        string output_path = RUN_PATH + index + ".out";
        conf->input_path = (char *)input_path.data();
        conf->output_path = (char *)output_path.data();

        // 运行程序
        run(conf, &res);

        // 判断结果
        JudgmentResult(&res, index);
    }
    return true;
}

// 判断单个测试用例结果
void Judger::JudgmentResult(struct result *res, string &index) {
    // 保存本次测试结果
    Json::Value testinfo;  // Json(Status, RunTime, RunMemory, StandardInput, StandardOutput, PersonalOutput)
    // 获取最大时间和空间
    m_runtime = max(m_runtime, res->cpu_time);
    m_runmemory = max(m_runmemory, res->memory);

    // 获取运行时间和运行内存的数据
    testinfo["RunTime"] = to_string(res->cpu_time) + "MS";
    testinfo["RunMemory"] = to_string(res->memory / 1024 / 1024) + "MB";

    // 获取标准输入
    ifstream infile;
    string indatapath = DATA_PATH + index + ".in";
    infile.open(indatapath.data());
    string standardinput((istreambuf_iterator<char>(infile)), (istreambuf_iterator<char>()));
    infile.close();
    // 获取标准答案
    ifstream infile1;
    string datapath = DATA_PATH + index + ".out";
    infile1.open(datapath.data());
    string standardanswer((istreambuf_iterator<char>(infile1)), (istreambuf_iterator<char>()));
    infile1.close();
    // 获取计算答案
    ifstream infile2;
    string runpath = RUN_PATH + index + ".out";
    infile2.open(runpath.data());
    string calculateanswer((istreambuf_iterator<char>(infile2)), (istreambuf_iterator<char>()));
    infile2.close();

    testinfo["StandardInput"] = standardinput;
    testinfo["StandardOutput"] = standardanswer;
    testinfo["PersonalOutput"] = calculateanswer;

    // 判断结果
    if (res->result == 0) {
        // 判断是否超出时间限制
        if (res->cpu_time > m_timelimit) {
            m_result = TLE;
            testinfo["Status"] = TLE;
        } else if (res->memory > m_memorylimit) {
            m_result = MLE;
            testinfo["Status"] = MLE;
        } else if (m_isspj) {  // SPJ 判断
            m_command = DATA_PATH + "spj " + indatapath + " " + datapath + " " + runpath;
            testinfo["Status"] = AC;  // 默认答案正确

            if (system(m_command.data()) != 0) {  // 如果答案错误
                m_result = WA;
                testinfo["Status"] = WA;
            }
        } else {                      // 普通判断
            testinfo["Status"] = AC;  // 默认答案正确
            // 比较答案（比较字符串）
            if (strcmp(standardanswer.data(), calculateanswer.data()) != 0) {
                testinfo["Status"] = WA;
                m_result = WA;
            }
        }
    } else if (res->result == 1) {  // CPU_TIME_LIMIT_EXCEEDED CPU 时间限制已超出
        m_result = TLE;
        testinfo["Status"] = TLE;
    } else if (res->result == 2) {  // REAL_TIME_LIMIT_EXCEEDED 真实时间限制已超出
        m_result = TLE;
        testinfo["Status"] = TLE;
    } else if (res->result == 3) {  // MEMORY_LIMIT_EXCEEDED 内存限制已超出
        m_result = MLE;
        testinfo["Status"] = MLE;
    } else if (res->result == 4) {  // RUNTIME_ERROR 运行时错误
        // 获取失败原因
        ifstream infile;
        m_command = RUN_PATH + "error.out";
        infile.open(m_command.data());

        string reason((istreambuf_iterator<char>(infile)), (istreambuf_iterator<char>()));

        m_reason = reason;
        m_result = RE;
        testinfo["Status"] = RE;
        infile.close();
    } else if (res->result == 5) {  // SYSTEM_ERROR 系统错误
        // 获取失败原因
        ifstream infile;
        m_command = RUN_PATH + "judger.log";
        infile.open(m_command.data());

        char reason[100];
        infile.getline(reason, 100);

        m_reason = reason;
        m_result = SE;
        testinfo["Status"] = SE;
        infile.close();
    }
    m_resjson["TestInfo"].append(testinfo);
}

// 结束函数
Json::Value Judger::Done() {
    if (m_result == PJ)
        m_result = AC;

    m_resjson["StatusRecordId"] = m_statusrecordid;
    m_resjson["Status"] = m_result;
    m_resjson["CompilerInfo"] = m_reason;
    m_resjson["RunTime"] = to_string(m_runtime) + "MS";
    m_resjson["RunMemory"] = to_string(int(m_runmemory / 1024 / 1024)) + "MB";
    m_resjson["Length"] = m_length;
    // 删除中间文件夹
    m_command = "rm -rf " + RUN_PATH;
    system(m_command.data());

    // 返回结果
    return m_resjson;
}
