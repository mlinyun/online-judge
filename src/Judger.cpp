#include "Judger.h"
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

extern "C"
{
#include "../include/runner.h"
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

Judger::Judger() {
}

Json::Value Judger::Run(Json::Value &runjson) {
    // 初始化数据
    if (!Init(runjson))
        return Done();

    // 编译 运行
    if (m_language == "C") {
        if (!CompileC())
            return Done();

        if (!RunProgramC_Cpp())
            return Done();
    } else if (m_language == "C++") {
        if (!CompileCpp())
            return Done();

        if (!RunProgramC_Cpp())
            return Done();
    } else if (m_language == "Go") {
        if (!CompileGo())
            return Done();

        if (!RunProgramGo())
            return Done();
    } else if (m_language == "Java") {
        if (!CompileJava())
            return Done();

        if (!RunProgramJava())
            return Done();
    } else if (m_language == "Python2") {
        if (!CompilePython2())
            return Done();

        if (!RunProgramPython2())
            return Done();
    } else if (m_language == "Python3") {
        if (!CompilePython3())
            return Done();

        if (!RunProgramPython3())
            return Done();
    } else if (m_language == "JavaScript") {
        if (!CompileJavaScript())
            return Done();

        if (!RunProgramJavaScript())
            return Done();
    }

    return Done();
}

bool Judger::Init(Json::Value &initjson) {
    // 初始化数据
    // Json(SubmitId,ProblemId,JudgeNum,Code,Language,TimeLimit,MemoryLimit)
    m_submitid = initjson["SubmitId"].asString();
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

    RUN_PATH = "./" + m_submitid + "/";
    DATA_PATH = "../../problemdata/" + m_problemid + "/";

    m_resjson.clear();

    // 创建中间文件夹
    m_command = "mkdir " + m_submitid;
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    // 将代码输出到文件中
    ofstream outfile;
    if (m_language == "C") {
        m_command = RUN_PATH + "main.c";
    } else if (m_language == "C++") {
        m_command = RUN_PATH + "main.cpp";
    } else if (m_language == "Go") {
        m_command = RUN_PATH + "main.go";
    } else if (m_language == "Java") {
        m_command = RUN_PATH + "Main.java";
    } else if (m_language == "Python2") {
        m_command = RUN_PATH + "main.py";
    } else if (m_language == "Python3") {
        m_command = RUN_PATH + "main.py";
    } else if (m_language == "JavaScript") {
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

    // 编译spj文件
    CompileSPJ();

    return true;
}

bool Judger::CompileSPJ() {
    // 如果存在可执行文件spj
    m_command = DATA_PATH + "spj";
    if (access(m_command.data(), F_OK) == 0) {
        m_isspj = true;
        return true;
    }

    // 如果不存在spj.cpp文件
    m_command = DATA_PATH + "spj.cpp";
    if (access(m_command.data(), F_OK) == -1)
        return true;

    // 编译SPJ文件
    m_command = "timeout 10 g++ " + DATA_PATH + "spj.cpp -o " + DATA_PATH + "spj -O2 -std=c++14";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    // 如果存在可执行文件spj
    m_command = DATA_PATH + "spj";
    if (access(m_command.data(), F_OK) == 0) {
        m_isspj = true;
        return true;
    }
    return true;
}

bool Judger::GetCompilationFailed() {
    ifstream infile;

    // 只要编译就会有这个文件
    m_command = RUN_PATH + "compileinfo.txt";
    infile.open(m_command.data());
    // 读取全部信息
    string reason((istreambuf_iterator<char>(infile)),
                  (istreambuf_iterator<char>()));
    m_reason = reason;
    m_result = CE;
    infile.close();

    return true;
}

bool Judger::CompileC() {
    // 进行gcc编译
    m_command =
            "timeout 10 gcc " + RUN_PATH + "main.c -fmax-errors=3 -o " + RUN_PATH + "main -O2 -std=c11 2>" + RUN_PATH +
            "compileinfo.txt";
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

bool Judger::CompileJava() {
    // 进行java编译
    m_command = "javac " + RUN_PATH + "Main.java -d " + RUN_PATH + "Main 2>" + RUN_PATH + "compileinfo.txt";
    if (system(m_command.data()) == -1) {
        m_result = SE;
        return false;
    }

    m_command = RUN_PATH + "Main";
    // 编译失败
    if (access(m_command.data(), F_OK) == -1) {
        // 返回编译失败原因
        GetCompilationFailed();
        return false;
    }
    return true;
}

bool Judger::CompilePython2() {
    // 进行Python2编译
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

bool Judger::CompilePython3() {
    // 进行Python3编译
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

bool Judger::CompileJavaScript() {
    // 进行JavaScript编译
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

bool Judger::RunProgramC_Cpp() {
    // 创建结构体
    struct config conf;

    conf.max_cpu_time = m_timelimit;
    conf.max_real_time = m_maxtimelimit;
    conf.max_memory = m_maxmemorylimie;

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = RUN_PATH + "main";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = (char *) "c_cpp";
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgramGo() {
    // 创建结构体
    struct config conf;

    conf.max_cpu_time = m_timelimit;
    conf.max_real_time = m_maxtimelimit;
    conf.max_memory = m_maxmemorylimie;

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = RUN_PATH + "main";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = (char *) "golang";
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));
    conf.env[0] = (char *) "LANG=en_US.UTF-8";
    conf.env[1] = (char *) "LANGUAGE=en_US:en";
    conf.env[2] = (char *) "LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgramJava() {
    // 创建结构体
    struct config conf;
    // Java的空间限制为其正确的五倍
    m_memorylimit = m_memorylimit * 3;

    conf.max_cpu_time = m_timelimit * 3;
    conf.max_real_time = m_maxtimelimit * 3;
    conf.max_memory = -1; // Java不能限制内存，在虚拟机中限制

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = "/usr/bin/java";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = nullptr;
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));

    string exe_file = RUN_PATH + "Main";
    string tmp_maxmemory = "-XX:MaxRAM=" + to_string(m_memorylimit) + "k";
    conf.args[0] = (char *) "/usr/bin/java";
    conf.args[1] = (char *) "-cp";
    conf.args[2] = (char *) exe_file.data();
    conf.args[3] = (char *) tmp_maxmemory.data();
    conf.args[4] = (char *) "-Djava.security.policy==policy";
    conf.args[5] = (char *) "-Djava.awt.headless=true";
    conf.args[6] = (char *) "Main";

    conf.env[0] = (char *) "LANG=en_US.UTF-8";
    conf.env[1] = (char *) "LANGUAGE=en_US:en";
    conf.env[2] = (char *) "LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgramPython2() {
    // 创建结构体
    struct config conf;

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = "/usr/bin/python2";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = (char *) "general";
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));

    string exe_file = RUN_PATH + "main.pyc";
    conf.args[0] = (char *) "/usr/bin/python2";
    conf.args[1] = (char *) exe_file.data();

    conf.env[0] = (char *) "LANG=en_US.UTF-8";
    conf.env[1] = (char *) "LANGUAGE=en_US:en";
    conf.env[2] = (char *) "LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgramPython3() {
    // 创建结构体
    struct config conf;

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = "/usr/bin/python3";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = (char *) "general";
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));

    string exe_file = RUN_PATH + "__pycache__/main.cpython-38.pyc";
    conf.args[0] = (char *) "/usr/bin/python3";
    conf.args[1] = (char *) exe_file.data();

    conf.env[0] = (char *) "LANG=en_US.UTF-8";
    conf.env[1] = (char *) "LANGUAGE=en_US:en";
    conf.env[2] = (char *) "LC_ALL=en_US.UTF-8";
    conf.env[3] = (char *) "PYTHONIOENCODING=utf-8";

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgramJavaScript() {
    // 创建结构体
    struct config conf;

    conf.max_cpu_time = m_timelimit * 2;
    conf.max_real_time = m_maxtimelimit * 2;
    conf.max_memory = m_maxmemorylimie * 2;

    conf.max_process_number = 200;
    conf.max_output_size = -1;
    conf.max_stack = 32 * 1024 * 1024;
    string exe_path = "/usr/bin/nodejs";
    conf.exe_path = (char *) exe_path.data();
    string error_path = RUN_PATH + "error.out";
    conf.error_path = (char *) error_path.data();
    string log_path = RUN_PATH + "judger.log";
    conf.log_path = (char *) log_path.data();
    conf.seccomp_rule_name = (char *) "node";
    conf.memory_limit_check_only = 1;
    conf.uid = 0;
    conf.gid = 0;
    memset(conf.args, 0, sizeof(conf.args));
    memset(conf.env, 0, sizeof(conf.env));

    string exe_file = RUN_PATH + "main.js";
    conf.args[0] = (char *) "/usr/bin/nodejs";
    conf.args[1] = (char *) exe_file.data();

    conf.env[0] = (char *) "LANG=en_US.UTF-8";
    conf.env[1] = (char *) "LANGUAGE=en_US:en";
    conf.env[2] = (char *) "LC_ALL=en_US.UTF-8";

    RunProgram(&conf);
    return true;
}

bool Judger::RunProgram(struct config *conf) {
    struct result res;
    // 根据测试数量进行判定
    for (int i = 1; i <= m_judgenum; i++) {
        string index = to_string(i);
        string input_path = DATA_PATH + index + ".in";
        conf->input_path = (char *) input_path.data();
        string output_path = RUN_PATH + index + ".out";
        conf->output_path = (char *) output_path.data();
        // 执行程序
        run(conf, &res);
        // printf("cpu time is %d,real time is %d,memory is %ld,signal is %d,result is %d\n", res.cpu_time, res.real_time, res.memory, res.signal, res.result);

        JudgmentResult(&res, index);
    }
    return true;
}

bool Judger::JudgmentResult(struct result *res, string &index) {
    Json::Value testinfo; // 保存本次结果
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
    string standardinput((istreambuf_iterator<char>(infile)),
                         (istreambuf_iterator<char>()));
    infile.close();
    // 获取标准答案
    ifstream infile1;
    string datapath = DATA_PATH + index + ".out";
    infile1.open(datapath.data());
    string standardanswer((istreambuf_iterator<char>(infile1)),
                          (istreambuf_iterator<char>()));
    infile1.close();
    // 获取计算答案
    ifstream infile2;
    string runpath = RUN_PATH + index + ".out";
    infile2.open(runpath.data());
    string calculateanswer((istreambuf_iterator<char>(infile2)),
                           (istreambuf_iterator<char>()));
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
        } else if (m_isspj) // SPJ判断
        {
            m_command = DATA_PATH + "spj " + indatapath + " " + datapath + " " + runpath;
            testinfo["Status"] = AC;

            if (system(m_command.data()) != 0) // 如果答案错误
            {
                testinfo["Status"] = WA;
                m_result = WA;
            }
        } else // 普通比较
        {
            testinfo["Status"] = AC;
            // 比较答案（比较字符串）
            if (strcmp(standardanswer.data(), calculateanswer.data()) != 0) {
                testinfo["Status"] = WA;
                m_result = WA;
            }
        }
    } else if (res->result == 1) // CPU_TIME_LIMIT_EXCEEDED
    {
        m_result = TLE;
        testinfo["Status"] = TLE;
    } else if (res->result == 2) // REAL_TIME_LIMIT_EXCEEDED
    {
        m_result = TLE;
        testinfo["Status"] = TLE;
    } else if (res->result == 3) // MEMORY_LIMIT_EXCEEDED
    {
        m_result = MLE;
        testinfo["Status"] = MLE;
    } else if (res->result == 4) {
        // 获取失败原因
        ifstream infile;
        m_command = RUN_PATH + "error.out";
        infile.open(m_command.data());

        string reason((istreambuf_iterator<char>(infile)),
                      (istreambuf_iterator<char>()));

        m_reason = reason;
        m_result = RE;
        testinfo["Status"] = RE;
        infile.close();
    } else if (res->result == 5) {
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

Json::Value Judger::Done() {
    // 返回：Json(Result,Reason,RunTime,RunMemory)
    if (m_result == PJ)
        m_result = AC;

    m_resjson["SubmitId"] = m_submitid;
    m_resjson["Status"] = m_result;
    m_resjson["ComplierInfo"] = m_reason;
    m_resjson["RunTime"] = to_string(m_runtime) + "MS";
    m_resjson["RunMemory"] = to_string(int(m_runmemory / 1024 / 1024)) + "MB";
    m_resjson["Length"] = m_length;
    // 删除中间文件夹
    m_command = "rm -rf " + RUN_PATH;
    system(m_command.data());

    return m_resjson;
}