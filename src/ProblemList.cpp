#include "ProblemList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;
const string PROBLEMDATAPATH = "../../problemdata/";

ProblemList *ProblemList::GetInstance() {
    static ProblemList problemlist;
    return &problemlist;
}

Json::Value ProblemList::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    // 获取基本信息
    Json::Value resjson = MoDB::GetInstance()->SelectProblemInfoByAdmin(queryjson);

    if (resjson["Result"].asString() == "Fail") {
        return resjson;
    }
    // 获取测试点信息
    string problemid = resjson["_id"].asString();
    int judgenum = stoi(resjson["JudgeNum"].asString());
    string DATA_PATH = PROBLEMDATAPATH + problemid + "/";
    ifstream infilein, infileout;
    for (int i = 1; i <= judgenum; i++) {
        Json::Value jsoninfo;
        string infilepath = DATA_PATH + to_string(i) + ".in";
        string outfilepath = DATA_PATH + to_string(i) + ".out";

        infilein.open(infilepath.data());
        string infile((istreambuf_iterator<char>(infilein)),
                      (istreambuf_iterator<char>()));

        infileout.open(outfilepath.data());
        string outfile((istreambuf_iterator<char>(infileout)),
                       (istreambuf_iterator<char>()));
        jsoninfo["in"] = infile;
        jsoninfo["out"] = outfile;

        infilein.close();
        infileout.close();

        resjson["TestInfo"].append(jsoninfo);
    }
    // 获取SPJ文件
    resjson["IsSPJ"] = false;
    string spjpath = DATA_PATH + "spj.cpp";
    if (access(spjpath.data(), F_OK) == 0) {
        ifstream infilespj;
        infilespj.open(spjpath.data());
        string spjfile((istreambuf_iterator<char>(infilespj)),
                       (istreambuf_iterator<char>()));
        resjson["SPJ"] = spjfile;
        resjson["IsSPJ"] = true;
        infilespj.close();
    }
    return resjson;
}

Json::Value ProblemList::SelectProblem(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectProblem(queryjson);
}

bool InsertProblemDataInfo(Json::Value &insertjson) {
    // 添加测试用例
    string DATA_PATH = PROBLEMDATAPATH + insertjson["ProblemId"].asString();
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
        outfilein << insertjson["TestInfo"][i - 1]["in"].asString();
        outfilein.close();

        outfileout.open(outpath.data());
        outfileout << insertjson["TestInfo"][i - 1]["out"].asString();
        outfileout.close();
    }
    // 添加SPJ文件
    if (insertjson["IsSPJ"].asBool()) {
        ofstream outfilespj;
        string spjpath = DATA_PATH + "/spj.cpp";
        outfilespj.open(spjpath.data());
        outfilespj << insertjson["SPJ"].asString();
        outfilespj.close();
    }
    return true;
}

Json::Value ProblemList::InsertProblem(Json::Value &insertjson) {
    Json::Value tmpjson = MoDB::GetInstance()->InsertProblem(insertjson);

    if (tmpjson["Result"] == "Fail") // 插入失败
        return tmpjson;

    // 插入信息
    Json::Value problemjson;
    problemjson["_id"] = tmpjson["ProblemId"];
    problemjson["Title"] = insertjson["Title"];
    problemjson["Description"] = insertjson["Description"];
    problemjson["JudgeNum"] = insertjson["JudgeNum"];
    problemjson["TimeLimit"] = insertjson["TimeLimit"];
    problemjson["MemoryLimit"] = insertjson["MemoryLimit"];

    insertjson["ProblemId"] = tmpjson["ProblemId"];

    InsertProblemDataInfo(insertjson);
    return tmpjson;
}

Json::Value ProblemList::UpdateProblem(Json::Value &updatejson) {
    Json::Value tmpjson = MoDB::GetInstance()->UpdateProblem(updatejson);
    if (tmpjson["Result"].asString() == "Fail")
        return tmpjson;

    string problemid = updatejson["ProblemId"].asString();
    string DATA_PATH = PROBLEMDATAPATH + problemid;
    // 删除文件夹
    string command = "rm -rf " + DATA_PATH;
    system(command.data());
    // 创建文件夹
    InsertProblemDataInfo(updatejson);

    return tmpjson;
}

Json::Value ProblemList::DeleteProblem(Json::Value &deletejson) {
    Json::Value tmpjson = MoDB::GetInstance()->DeleteProblem(deletejson);

    if (tmpjson["Result"] == "Fail")
        return tmpjson;

    // 删除数据
    string DATA_PATH = PROBLEMDATAPATH + deletejson["ProblemId"].asString();
    string command = "rm -rf " + DATA_PATH;

    system(command.data());

    return tmpjson;
}

Json::Value ProblemList::SelectProblemList(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectProblemList(queryjson);
}

Json::Value ProblemList::SelectProblemListByAdmin(Json::Value &queryjson) {
    return MoDB::GetInstance()->SelectProblemListByAdmin(queryjson);
}

bool ProblemList::UpdateProblemStatusNum(Json::Value &updatejson) {
    return MoDB::GetInstance()->UpdateProblemStatusNum(updatejson);
}

ProblemList::ProblemList() {
}

ProblemList::~ProblemList() {
}