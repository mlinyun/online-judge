#include "ProblemList.h"
#include "MongoDataBase.h"
#include "RedisDataBase.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

/*
    PROBLEMDATAPATH 变量表示 problemdata 文件夹的相对路径
    当题目增删改时，problemdata 中的数据文件也会相应改变
 */
const string PROBLEMDATAPATH = "../problemdata/";

// 管理员查询题目详细信息
Json::Value ProblemList::SelectProblemInfoByAdmin(Json::Value &queryjson)
{
    // 获取基本信息
    Json::Value resjson = MoDB::GetInstance()->SelectProblemInfoByAdmin(queryjson);

    if (resjson["Result"].asString() == "Fail")
    {
        return resjson;
    }
    // 获取测试点信息
    string problemid = resjson["_id"].asString();
    int judgenum = stoi(resjson["JudgeNum"].asString());
    string DATA_PATH = PROBLEMDATAPATH + problemid + "/";
    ifstream infilein, infileout;
    for (int i = 1; i <= judgenum; i++)
    {
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
    if (access(spjpath.data(), F_OK) == 0)
    {
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

// 用户查询题目详细信息（redis 缓存）
Json::Value ProblemList::SelectProblem(Json::Value &queryjson)
{
    string problemid = queryjson["ProblemId"].asString();

    // 获取缓存
    string resstr = ReDB::GetInstance()->GetProblemCache(problemid);

    Json::Value resjson;
    Json::Reader reader;
    // 如果有缓存
    if (resstr != "")
    {
        // 解析缓存json
        reader.parse(resstr, resjson);

        return resjson;
    }
    // 如果没有缓存
    resjson = MoDB::GetInstance()->SelectProblem(queryjson);

    // 添加缓存
    if (resjson["Result"].asString() == "Success")
    {
        ReDB::GetInstance()->AddProblemCache(problemid, resjson.toStyledString());
    }

    return resjson;
}

// 插入题目数据信息
bool InsertProblemDataInfo(Json::Value &insertjson)
{
    // 添加测试用例
    string DATA_PATH = PROBLEMDATAPATH + insertjson["ProblemId"].asString();
    string command = "mkdir " + DATA_PATH;
    // 创建文件夹
    system(command.data());
    // 添加测试文件
    for (int i = 1; i <= insertjson["TestInfo"].size(); i++)
    {
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
    if (insertjson["IsSPJ"].asBool())
    {
        ofstream outfilespj;
        string spjpath = DATA_PATH + "/spj.cpp";
        outfilespj.open(spjpath.data());
        outfilespj << insertjson["SPJ"].asString();
        outfilespj.close();
    }
    return true;
}

// 插入题目
Json::Value ProblemList::InsertProblem(Json::Value &insertjson)
{
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

// 修改题目信息
Json::Value ProblemList::UpdateProblem(Json::Value &updatejson)
{
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

    // 删除缓存
    ReDB::GetInstance()->DeleteProblemCache(problemid);
    return tmpjson;
}

// 删除题目
Json::Value ProblemList::DeleteProblem(Json::Value &deletejson)
{
    Json::Value tmpjson = MoDB::GetInstance()->DeleteProblem(deletejson);

    if (tmpjson["Result"] == "Fail")
        return tmpjson;

    // 删除数据
    string DATA_PATH = PROBLEMDATAPATH + deletejson["ProblemId"].asString();
    string command = "rm -rf " + DATA_PATH;

    system(command.data());

    // 删除缓存
    ReDB::GetInstance()->DeleteProblemCache(deletejson["ProblemId"].asString());
    return tmpjson;
}

// 通过普通查询获取题库数据
Json::Value ProblemList::SelectProblemList(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectProblemList(queryjson);
}

// 管理员查询列表
Json::Value ProblemList::SelectProblemListByAdmin(Json::Value &queryjson)
{
    return MoDB::GetInstance()->SelectProblemListByAdmin(queryjson);
}

// 更新题目的状态数量
bool ProblemList::UpdateProblemStatusNum(Json::Value &updatejson)
{
    return MoDB::GetInstance()->UpdateProblemStatusNum(updatejson);
}

ProblemList *ProblemList::GetInstance()
{
    static ProblemList problemlist;
    return &problemlist;
}

ProblemList::ProblemList()
{
}

ProblemList::~ProblemList()
{
}
