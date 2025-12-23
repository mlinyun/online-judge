#include "services/problem_service.h"

#include <unistd.h>
#include <utils/json_utils.h>

#include <fstream>

#include "constants/judge.h"
#include "db/mongo_database.h"
#include "db/redis_database.h"
#include "utils/response.h"

/**
 * 存储题目数据的路径前缀
 */
using constants::judge::PROBLEM_DATA_PREFIX;

// 局部静态特性的方式实现单实例模式
ProblemService *ProblemService::GetInstance() {
    static ProblemService problem_service;
    return &problem_service;
}

// 查询题目信息（单条）（Redis 缓存）
Json::Value ProblemService::SelectProblemInfo(Json::Value &queryjson) {
    // 获取题目 ID
    string problemid = queryjson["ProblemId"].asString();
    // 获取缓存
    string problem_cache = ReDB::GetInstance()->GetProblemCache(problemid);
    Json::Value resjson;
    Json::Reader reader;
    // 如果有缓存
    if (problem_cache != "") {
        // 解析缓存 Json
        reader.parse(problem_cache, resjson);
        return resjson;
    }
    // 如果没有缓存
    resjson = MoDB::GetInstance()->SelectProblemInfo(queryjson);
    // 添加缓存
    if (resjson["success"].asBool()) {
        string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
        ReDB::GetInstance()->AddProblemCache(problemid, resbody);
    }
    // 返回结果
    return resjson;
}

// 查询题目信息（管理员权限）
Json::Value ProblemService::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    // 获取基本信息
    Json::Value resjson = MoDB::GetInstance()->SelectProblemInfoByAdmin(queryjson);
    if (!resjson["success"].asBool()) {
        return resjson;
    }
    // 获取测试点信息
    Json::Value &data = resjson["data"];
    string problemid = data["_id"].asString();
    int judgenum = stoi(data["JudgeNum"].asString());
    string DATA_PATH = PROBLEM_DATA_PREFIX + problemid + "/";
    ifstream infilein, infileout;
    for (int i = 1; i <= judgenum; i++) {
        Json::Value jsoninfo;
        string infilepath = DATA_PATH + to_string(i) + ".in";
        string outfilepath = DATA_PATH + to_string(i) + ".out";

        infilein.open(infilepath.data());
        string infile((istreambuf_iterator<char>(infilein)), (istreambuf_iterator<char>()));

        infileout.open(outfilepath.data());
        string outfile((istreambuf_iterator<char>(infileout)), (istreambuf_iterator<char>()));
        jsoninfo["Input"] = infile;
        jsoninfo["Output"] = outfile;

        infilein.close();
        infileout.close();

        data["TestInfo"].append(jsoninfo);
    }
    // 获取 SPJ 文件
    data["IsSPJ"] = false;
    string spjpath = DATA_PATH + "spj.cpp";
    if (access(spjpath.data(), F_OK) == 0) {
        ifstream infilespj;
        infilespj.open(spjpath.data());
        string spjfile((istreambuf_iterator<char>(infilespj)), (istreambuf_iterator<char>()));
        data["SPJ"] = spjfile;
        data["IsSPJ"] = true;
        infilespj.close();
    }
    return resjson;
}

// 插入题目测试数据
bool InsertProblemDataInfo(Json::Value &insertjson) {
    // 添加测试用例
    string DATA_PATH = PROBLEM_DATA_PREFIX + insertjson["ProblemId"].asString();
    string command = "mkdir " + DATA_PATH;
    // 创建文件夹
    system(command.data());
    // 添加测试文件
    for (int i = 1; i <= insertjson["TestInfo"].size(); i++) {
        string index = to_string(i);
        ofstream outfilein, outfileout;
        string inpath = DATA_PATH + "/" + index + ".in";
        string outpath = DATA_PATH + "/" + index + ".out";

        outfilein.open(inpath.data());
        outfilein << insertjson["TestInfo"][i - 1]["Input"].asString();
        outfilein.close();

        outfileout.open(outpath.data());
        outfileout << insertjson["TestInfo"][i - 1]["Output"].asString();
        outfileout.close();
    }
    // 添加 SPJ 文件
    if (insertjson["IsSPJ"].asBool()) {
        ofstream outfilespj;
        string spjpath = DATA_PATH + "/spj.cpp";
        outfilespj.open(spjpath.data());
        outfilespj << insertjson["SPJ"].asString();
        outfilespj.close();
    }
    return true;
}

// 插入题目（管理员权限）
Json::Value ProblemService::InsertProblem(Json::Value &insertjson) {
    Json::Value tmpjson = MoDB::GetInstance()->InsertProblem(insertjson);
    if (!tmpjson["success"].asBool()) {
        // 插入失败，直接返回，不进行后续操作
        return tmpjson;
    }
    // 插入题目测试数据
    Json::Value &data = tmpjson["data"];          // 获取插入题目成功后返回的数据（即题目 ID）
    insertjson["ProblemId"] = data["ProblemId"];  // 设置题目 ID
    InsertProblemDataInfo(insertjson);
    return tmpjson;
}

// 更新题目信息（管理员权限）
Json::Value ProblemService::UpdateProblem(Json::Value &updatejson) {
    Json::Value tmpjson = MoDB::GetInstance()->UpdateProblem(updatejson);
    if (!tmpjson["success"].asBool()) {
        // 更新失败，直接返回，不进行后续操作
        return tmpjson;
    }
    // 获取题目 ID
    string problemid = updatejson["ProblemId"].asString();
    string DATA_PATH = PROBLEM_DATA_PREFIX + problemid;
    // 删除文件夹
    string command = "rm -rf " + DATA_PATH;
    system(command.data());
    // 创建文件夹
    InsertProblemDataInfo(updatejson);
    // 删除缓存
    ReDB::GetInstance()->DeleteProblemCache(problemid);
    return tmpjson;
}

// 删除题目（管理员权限）
Json::Value ProblemService::DeleteProblem(Json::Value &deletejson) {
    Json::Value tmpjson = MoDB::GetInstance()->DeleteProblem(deletejson);
    if (!tmpjson["success"].asBool()) {
        return tmpjson;
    }
    // 删除数据
    string DATA_PATH = PROBLEM_DATA_PREFIX + deletejson["ProblemId"].asString();
    string command = "rm -rf " + DATA_PATH;
    system(command.data());
    // 删除缓存
    ReDB::GetInstance()->DeleteProblemCache(deletejson["ProblemId"].asString());
    return tmpjson;
}

// 分页获取题目列表
Json::Value ProblemService::SelectProblemList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectProblemList(queryjson);
}

// 分页获取题目列表（管理员权限）
Json::Value ProblemService::SelectProblemListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectProblemListByAdmin(queryjson);
}

// 更新题目的状态数量
bool ProblemService::UpdateProblemStatusNum(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateProblemStatusNum(updatejson);
}

ProblemService::ProblemService() {
    // 构造函数实现
}

ProblemService::~ProblemService() {
    // 析构函数实现
}