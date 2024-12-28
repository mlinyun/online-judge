#include "MongoDataBase.h"
#include <cstdint>
#include <iostream>
#include <ctime>
#include "./utils/snowflake.hpp" // 雪花算法
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/pipeline.hpp>

#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace std;

// 雪花算法
using snowflake_t = snowflake<1534832906275L, std::mutex>;
snowflake_t uuid;

/*
    获取时间
*/
string GetTime()
{
    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    char str[50];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", now);
    return (string)str;
}

/*
    功能：获取某一个集合中最大的ID
*/
int64_t MoDB::GetMaxId(std::string name)
{
    auto client = pool.acquire();
    mongocxx::collection coll = (*client)["OnlineJudge"][name.data()];

    bsoncxx::builder::stream::document document{};
    mongocxx::pipeline pipe;
    pipe.sort({make_document(kvp("_id", -1))});
    pipe.limit(1);
    mongocxx::cursor cursor = coll.aggregate(pipe);

    // 如果没找到，说明集合中没有数据
    if (cursor.begin() == cursor.end())
        return 0;

    Json::Value jsonvalue;
    Json::Reader reader;
    for (auto doc : cursor)
    {
        reader.parse(bsoncxx::to_json(doc), jsonvalue);
    }

    int64_t id = stoll(jsonvalue["_id"].asString());
    return id;
}

MoDB *MoDB::GetInstance()
{
    static MoDB modb;
    return &modb;
}

// ++++++++++++++++++++++++++++++ 用户模块 Start ++++++++++++++++++++++++++++++
/*
    功能：用户注册，首先需要检查账户是否存在，其次检查昵称是否存在，如果都不存在则直接插入数据
    传入：Json(NickName, Account, PassWord, PersonalProfile, School, Major)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::RegisterUser(Json::Value &registerjson)
{
    Json::Value resjson;
    try
    {
        string account = registerjson["Account"].asString();
        string nickname = registerjson["NickName"].asString();
        string password = registerjson["PassWord"].asString();
        string personalprofile = registerjson["PersonalProfile"].asString();
        string school = registerjson["School"].asString();
        string major = registerjson["Major"].asString();

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];
        mongocxx::cursor cursor = usercoll.find({make_document(kvp("Account", account.data()))});
        // 判断账户是否存在
        if (cursor.begin() != cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "账户已存在，请重新填写！";
            return resjson;
        }
        // 检查昵称是否存在
        cursor = usercoll.find({make_document(kvp("NickName", nickname.data()))});
        if (cursor.begin() != cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "昵称已存在，请重新填写！";
            return resjson;
        }

        uuid.init(1, 1);
        int64_t id = uuid.nextid();
        string jointime = GetTime();
        // 默认头像
        string avatar = "http://127.0.0.1:8081/image/1";
        // 插入
        bsoncxx::builder::stream::document document{};
        document
            << "_id" << id
            << "Avatar" << avatar.data()
            << "NickName" << nickname.data()
            << "Account" << account.data()
            << "PassWord" << password.data()
            << "PersonalProfile" << personalprofile.data()
            << "School" << school.data()
            << "Major" << major.data()
            << "JoinTime" << jointime.data()
            << "CommentLikes" << open_array << close_array
            << "Solves" << open_array << close_array
            << "SubmitNum" << 0
            << "ACNum" << 0
            << "Authority" << 3;

        auto result = usercoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["Reason"] = "注册成功！";
        resjson["_id"] = (Json::Int64)id;
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：用户登录，将账号和密码作为查询条件，如果未查出数据，则代表账号密码错误或没有该账号，如果查询出数据，直接返回即可
    传入：Json(Account, PassWord)
    传出：Json(Result, Reason, Info(_id, NickName, Avatar, CommentLikes, Solves, Authority))
*/
Json::Value MoDB::LoginUser(Json::Value &loginjson)
{
    Json::Value resjson;
    try
    {
        string account = loginjson["Account"].asString();
        string password = loginjson["PassWord"].asString();

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        document
            << "Account" << account.data()
            << "PassWord" << password.data();
        pipe.match(document.view());

        document.clear();
        document
            << "Avatar" << 1
            << "NickName" << 1
            << "CommentLikes" << 1
            << "Solves" << 1
            << "Authority" << 1;

        pipe.project(document.view());
        // 匹配账号和密码
        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        // 匹配失败
        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "账户或密码错误！";
            return resjson;
        }
        // 匹配成功
        Json::Reader reader;
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["Info"] = jsonvalue;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：用户主页展示，直接返回用户数据即可
    传入：Json(UserId)
    传出：Json(Result, Reason, _id, Avatar, NickName, PersonalProfile, School, Major, JoinTime, Solves, ACNum, SubmitNum)
*/
Json::Value MoDB::SelectUserInfo(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t userid = stoll(queryjson["UserId"].asString());
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;

        pipe.match({make_document(kvp("_id", userid))});

        document
            << "Avatar" << 1
            << "NickName" << 1
            << "PersonalProfile" << 1
            << "School" << 1
            << "Major" << 1
            << "JoinTime" << 1
            << "Solves" << 1
            << "ACNum" << 1
            << "SubmitNum" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：更改用户信息，直接修改即可
    传入：Json(UserId, Avatar, PersonalProfile, School, Major)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::UpdateUserInfo(Json::Value &updatejson)
{
    Json::Value resjson;
    try
    {
        int64_t userid = stoll(updatejson["UserId"].asString());
        string avatar = updatejson["Avatar"].asString();
        string personalprofile = updatejson["PersonalProfile"].asString();
        string school = updatejson["School"].asString();
        string major = updatejson["Major"].asString();

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        bsoncxx::builder::stream::document document{};
        document
            << "$set" << open_document
            << "Avatar" << avatar.data()
            << "PersonalProfile" << personalprofile.data()
            << "School" << school.data()
            << "Major" << major.data()
            << close_document;

        usercoll.update_one({make_document(kvp("_id", userid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：查询用户表，用于设置页面修改用户，直接返回相应的用户信息即可
    传入：Json(UserId)
    传出：Json(Result, Reason, _id, Avatar, NickName, PersonalProfile, School, Major)
*/
Json::Value MoDB::SelectUserUpdateInfo(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t userid = stoll(queryjson["UserId"].asString());
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;

        pipe.match({make_document(kvp("_id", userid))});
        document
            << "Avatar" << 1
            << "NickName" << 1
            << "PersonalProfile" << 1
            << "School" << 1
            << "Major" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：分页查询用户列表
    传入：Json(Page,PageSize)
    传出：Json(Result, Reason, ArrayInfo[_id, NickName, PersonalProfile, School, Major, JoinTime], TotalNum)
*/
Json::Value MoDB::SelectUserSetInfo(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.skip(skip);
        pipe.limit(pagesize);

        document
            << "NickName" << 1
            << "PersonalProfile" << 1
            << "School" << 1
            << "Major" << 1
            << "JoinTime" << 1;

        pipe.project(document.view());
        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        resjson["TotalNum"] = to_string(usercoll.count_documents({}));
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：删除用户，匹配 ID 直接删除
    传入：Json(UserId)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::DeleteUser(Json::Value &deletejson)
{
    Json::Value resjson;
    try
    {
        int64_t userid = stoll(deletejson["UserId"].asString());
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        auto result = usercoll.delete_one({make_document(kvp("_id", userid))});

        if ((*result).deleted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：更新用户的题目信息（当用户提交代码时），当用户提交代码时，更新用户的题目信息。用户提交代码后，首先将提交次数加 1，
          如果代码通过了，检查 Solves 数组是否有该 ID，如果没有代表第一次通过，会将通过次数加 1 并将 ID 加入 Solves 数组
    传入：Json(UserId, ProblemId, Status)
    传出：bool(如果 AC 是否向其添加)
*/
bool MoDB::UpdateUserProblemInfo(Json::Value &updatejson)
{
    try
    {
        int64_t userid = stoll(updatejson["UserId"].asString());
        int problemid = stoi(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        // 将用户提交数目加一
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];

        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "SubmitNum" << 1 << close_document;

        usercoll.update_one({make_document(kvp("_id", userid))}, document.view());

        // 如果 AC 了
        if (status == 2)
        {
            // 查询 AC 题目是否已经添加至 Solves 的数组中
            mongocxx::pipeline pipe;
            pipe.match({make_document(kvp("_id", userid))});
            document.clear();
            document
                << "IsHasAc" << open_document
                << "$in" << open_array << problemid << "$Solves" << close_array
                << close_document;
            pipe.project(document.view());
            Json::Reader reader;
            Json::Value tmpjson;
            mongocxx::cursor cursor = usercoll.aggregate(pipe);

            for (auto doc : cursor)
            {
                reader.parse(bsoncxx::to_json(doc), tmpjson);
            }
            // 如果未添加
            if (tmpjson["IsHasAc"].asBool() == false)
            {
                document.clear();
                document
                    << "$push" << open_document
                    << "Solves" << problemid
                    << close_document
                    << "$inc" << open_document
                    << "ACNum" << 1 << close_document;
                usercoll.update_one({make_document(kvp("_id", userid))}, document.view());
                return true;
            }
            return false;
        }
        return false;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

/*
    功能：获取用户的 Rank 排名数组，用于排名页面展示，先通过 AC 题目反序排序，在通过提交数量正序排序
    传入：Json(Page, PageSize)
    传出：Json(Result, Reason, ArrayInfo[_id, Rank, Avatar, NickName, PersonalProfile, SubmitNum, ACNum], TotalNum)
*/
Json::Value MoDB::SelectUserRank(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)["OnlineJudge"]["User"];
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;
        Json::Reader reader;

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = usercoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        document
            << "ACNum" << -1
            << "SubmitNum" << 1;

        pipe.sort(document.view());
        pipe.skip(skip);
        pipe.limit(pagesize);
        document.clear();
        document
            << "Avatar" << 1
            << "NickName" << 1
            << "PersonalProfile" << 1
            << "SubmitNum" << 1
            << "ACNum" << 1;
        pipe.project(document.view());

        cursor = usercoll.aggregate(pipe);
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        // 添加 Rank 排名
        int currank = (page - 1) * pagesize + 1;
        for (int i = 0; i < resjson["ArrayInfo"].size(); i++)
        {
            resjson["ArrayInfo"][i]["Rank"] = currank++;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
// ++++++++++++++++++++++++++++++ 用户模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 题目模块 Start ++++++++++++++++++++++++++++++
/*
    功能：管理员查询题目详细信息
    传入：Json(ProblemId)
    传出：Json(Result, Reason, _id, Title, Description, TimeLimit, MemoryLimit, JudgeNum, UserNickName, Tags)
*/
Json::Value MoDB::SelectProblemInfoByAdmin(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t problemid = stoll(queryjson["ProblemId"].asString());

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", problemid))});

        document
            << "Title" << 1
            << "Description" << 1
            << "TimeLimit" << 1
            << "MemoryLimit" << 1
            << "JudgeNum" << 1
            << "UserNickName" << 1
            << "Tags" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = problemcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：用户查询题目详细信息
    传入：Json(ProblemId)
    传出：Json(Result, Reason, _id, Title, Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum, ACNum, UserNickName, Tags)
*/
Json::Value MoDB::SelectProblem(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t problemid = stoll(queryjson["ProblemId"].asString());

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", problemid))});

        document
            << "Title" << 1
            << "Description" << 1
            << "TimeLimit" << 1
            << "MemoryLimit" << 1
            << "JudgeNum" << 1
            << "SubmitNum" << 1
            << "ACNum" << 1
            << "UserNickName" << 1
            << "Tags" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = problemcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}

/*
    功能：插入题目
    传入：Json(Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
    传出：Json(Reuslt, Reason, ProblemId)
*/
Json::Value MoDB::InsertProblem(Json::Value &insertjson)
{
    Json::Value resjson;
    try
    {
        int64_t problemid = ++m_problemid;
        string title = insertjson["Title"].asString();
        string description = insertjson["Description"].asString();
        int timelimit = stoi(insertjson["TimeLimit"].asString());
        int memorylimit = stoi(insertjson["MemoryLimit"].asString());
        int judgenum = stoi(insertjson["JudgeNum"].asString());
        string usernickname = insertjson["UserNickName"].asString();

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        bsoncxx::builder::stream::document document{};
        auto in_array = document
                        << "_id" << problemid
                        << "Title" << title.data()
                        << "Description" << description.data()
                        << "TimeLimit" << timelimit
                        << "MemoryLimit" << memorylimit
                        << "JudgeNum" << judgenum
                        << "SubmitNum" << 0
                        << "CENum" << 0
                        << "ACNum" << 0
                        << "WANum" << 0
                        << "RENum" << 0
                        << "TLENum" << 0
                        << "MLENum" << 0
                        << "SENum" << 0
                        << "UserNickName" << usernickname.data()
                        << "Tags" << open_array;
        for (int i = 0; i < insertjson["Tags"].size(); i++)
        {
            string tag = insertjson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << finalize;
        auto result = problemcoll.insert_one(doc.view());

        if ((*result).result().inserted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["ProblemId"] = (Json::Int64)problemid;
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：修改题目信息
    传入：Json(ProblemId, Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::UpdateProblem(Json::Value &updatejson)
{
    Json::Value resjson;
    try
    {
        int problemid = stoi(updatejson["ProblemId"].asString());
        string title = updatejson["Title"].asString();
        string description = updatejson["Description"].asString();
        int timelimit = stoi(updatejson["TimeLimit"].asString());
        int memorylimit = stoi(updatejson["MemoryLimit"].asString());
        int judgenum = stoi(updatejson["JudgeNum"].asString());
        string usernickname = updatejson["UserNickName"].asString();

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        bsoncxx::builder::stream::document document{};
        auto in_array = document
                        << "$set" << open_document
                        << "Title" << title.data()
                        << "Description" << description.data()
                        << "TimeLimit" << timelimit
                        << "MemoryLimit" << memorylimit
                        << "JudgeNum" << judgenum
                        << "UserNickName" << usernickname.data()
                        << "Tags" << open_array;
        for (int i = 0; i < updatejson["Tags"].size(); i++)
        {
            string tag = updatejson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << close_document << finalize;

        problemcoll.update_one({make_document(kvp("_id", problemid))}, doc.view());

        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：删除题目
    传入：Json(ProblemId)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::DeleteProblem(Json::Value &deletejson)
{
    Json::Value resjson;
    try
    {
        int64_t problemid = stoll(deletejson["ProblemId"].asString());

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        auto result = problemcoll.delete_one({make_document(kvp("_id", problemid))});

        if ((*result).deleted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：将字符串变为数字
    主要为查询ID服务，限制：ID长度不能大于4，只关注数字
*/
int mystoi(string num)
{
    int resnum = 0;
    if (num.size() >= 4 || num.size() == 0)
        return resnum;
    for (auto n : num)
    {
        if (isdigit(n))
        {
            resnum = resnum * 10 + n - '0';
        }
    }
    return resnum;
}

/*
    功能：将字符串变为 int64
    主要为查询 ID 服务，限制：ID 长度不能大于 19，只关注数字
*/
int64_t mystoll(string num)
{
    int64_t resnum = 0;
    if (num.size() >= 19 || num.size() == 0)
        return resnum;
    for (auto n : num)
    {
        if (isdigit(n))
        {
            resnum = resnum * 10 + n - '0';
        }
    }
    return resnum;
}

/*
    功能：分页获取题目列表
    传入：Json(SearchInfo{Id, Title, Tags[]}, Page, PageSize)
    传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum, TLENum, MLENum, SENum, Tags]), TotalNum)
*/
Json::Value MoDB::SelectProblemList(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        Json::Value searchinfo = queryjson["SearchInfo"];
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];
        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 查询ID
        if (searchinfo["Id"].asString().size() > 0)
        {
            int id = mystoi(searchinfo["Id"].asString());
            pipe.match({make_document(kvp("_id", id))});
            pipetot.match({make_document(kvp("_id", id))});
        }

        // 查询标题
        if (searchinfo["Title"].asString().size() > 0)
        {
            document
                << "Title" << open_document
                << "$regex" << searchinfo["Title"].asString()
                << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询标签
        if (searchinfo["Tags"].size() > 0)
        {
            auto in_array = document
                            << "Tags" << open_document
                            << "$in" << open_array;
            for (int i = 0; i < searchinfo["Tags"].size(); i++)
            {
                in_array = in_array
                           << searchinfo["Tags"][i].asString();
            }
            bsoncxx::document::value doc = in_array << close_array << close_document << finalize;
            pipe.match(doc.view());
            pipetot.match(doc.view());
            document.clear();
        }
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行
        document
            << "ProblemId"
            << "$_id"
            << "Title" << 1
            << "SubmitNum" << 1
            << "CENum" << 1
            << "ACNum" << 1
            << "WANum" << 1
            << "RENum" << 1
            << "TLENum" << 1
            << "MLENum" << 1
            << "SENum" << 1
            << "Tags" << 1;
        pipe.project(document.view());

        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Reuslt"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}

/*
    功能：管理员分页获取题目列表
    传入：Json(Page, PageSize)
    传出：Json(Result, Reason, ArrayInfo([ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum, TLENum, MLENum, SENum, Tags]), TotalNum)
*/
Json::Value MoDB::SelectProblemListByAdmin(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];

        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行
        document
            << "ProblemId"
            << "$_id"
            << "Title" << 1
            << "SubmitNum" << 1
            << "CENum" << 1
            << "ACNum" << 1
            << "WANum" << 1
            << "RENum" << 1
            << "TLENum" << 1
            << "MLENum" << 1
            << "SENum" << 1
            << "Tags" << 1;
        pipe.project(document.view());

        Json::Value arryjson;
        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            arryjson.append(jsonvalue);
        }
        resjson["ArrayInfo"] = arryjson;
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}
/*
    功能：更新题目的状态数量
    传入：Json(ProblemId, Status)
    传出：bool
*/
bool MoDB::UpdateProblemStatusNum(Json::Value &updatejson)
{
    try
    {
        int64_t problemid = stoll(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        string statusnum = "";
        if (status == 1)
            statusnum = "CENum";
        else if (status == 2)
            statusnum = "ACNum";
        else if (status == 3)
            statusnum = "WANum";
        else if (status == 4)
            statusnum = "RENum";
        else if (status == 5)
            statusnum = "TLENum";
        else if (status == 6)
            statusnum = "MLENum";
        else if (status == 7)
            statusnum = "SENum";

        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];
        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "SubmitNum" << 1
            << statusnum << 1 << close_document;

        problemcoll.update_one({make_document(kvp("_id", problemid))}, document.view());
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

/*
    功能：获取题目的所有标签
    传入：void
    传出：Json(tags)
*/
Json::Value MoDB::getProblemTags()
{
    auto client = pool.acquire();
    mongocxx::collection problemcoll = (*client)["OnlineJudge"]["Problem"];
    mongocxx::cursor cursor = problemcoll.distinct({"Tags"}, {});
    Json::Reader reader;
    Json::Value resjson;
    for (auto doc : cursor)
    {
        reader.parse(bsoncxx::to_json(doc), resjson);
    }
    return resjson;
}
// ++++++++++++++++++++++++++++++ 题目模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 公告模块 Start ++++++++++++++++++++++++++++++
/*
    功能：添加公告
    传入：Json(Title, Content, UserId, Level)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::InsertAnnouncement(Json::Value &insertjson)
{
    Json::Value resjson;
    try
    {
        int64_t id = ++m_articleid;
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        int level = stoi(insertjson["Level"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];
        bsoncxx::builder::stream::document document{};
        document
            << "_id" << id
            << "Title" << title.data()
            << "Content" << content.data()
            << "UserId" << userid
            << "Views" << 0
            << "Comments" << 0
            << "Level" << level
            << "CreateTime" << GetTime().data()
            << "UpdateTime" << GetTime().data();

        auto result = announcementcoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
/*
    功能：分页查询公告
    传入：Json(Page, PageSize)
    传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime], TotalNum)
*/
Json::Value MoDB::SelectAnnouncementList(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = announcementcoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.sort({make_document(kvp("Level", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);

        document
            << "Title" << 1
            << "Views" << 1
            << "Comments" << 1
            << "CreateTime" << 1;
        pipe.project(document.view());

        cursor = announcementcoll.aggregate(pipe);

        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：查询公告的详细信息，主要是编辑时的查询
    传入：Json(AnnouncementId)
    传出：Json(Result, Reason, Title, Content, Level)
*/
Json::Value MoDB::SelectAnnouncementByEdit(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t announcementid = stoll(queryjson["AnnouncementId"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", announcementid))});
        document
            << "Title" << 1
            << "Content" << 1
            << "Level" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = announcementcoll.aggregate(pipe);

        Json::Reader reader;
        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！,可能是请求参数出错！";
            return resjson;
        }
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
/*
    功能：查询公告的详细内容，并且将其浏览量加一
    传入：Json(AnnouncementId)
    传出：Json(Title, Content, Views, Comments, CreateTime, UpdateTime)
*/
Json::Value MoDB::SelectAnnouncement(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t announcementid = stoll(queryjson["AnnouncementId"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];
        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "Views" << 1 << close_document;
        announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());

        // 查询
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", announcementid))});
        document.clear();
        document
            << "Title" << 1
            << "Content" << 1
            << "Views" << 1
            << "Comments" << 1
            << "CreateTime" << 1
            << "UpdateTime" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = announcementcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据，可能是请求参数出错！";
            return resjson;
        }

        Json::Reader reader;
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：修改公告的评论数
    传入：Json(ArticleId, Num)
    传出：bool
*/
bool MoDB::UpdateAnnouncementComments(Json::Value &updatejson)
{
    try
    {
        int64_t articleid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];

        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "Comments" << num << close_document;
        announcementcoll.update_one({make_document(kvp("_id", articleid))}, document.view());
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

/*
    功能：更新公告
    传入：Json(AnnouncementId, Title, ontent, Level)
    传出；Json(Result,Reason)
*/
Json::Value MoDB::UpdateAnnouncement(Json::Value &updatejson)
{
    Json::Value resjson;
    try
    {
        int64_t announcementid = stoll(updatejson["AnnouncementId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();
        int level = stoi(updatejson["Level"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];

        bsoncxx::builder::stream::document document{};
        document
            << "$set" << open_document
            << "Title" << title.data()
            << "Content" << content.data()
            << "Level" << level
            << "UpdateTime" << GetTime().data()
            << close_document;

        announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：删除公告
    传入：Json(AnnouncementId)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::DeleteAnnouncement(Json::Value &deletejson)
{
    Json::Value resjson;
    try
    {
        int64_t announcementid = stoll(deletejson["AnnouncementId"].asString());

        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)["OnlineJudge"]["Announcement"];

        auto result = announcementcoll.delete_one({make_document(kvp("_id", announcementid))});
        if ((*result).deleted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库删除失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
// ++++++++++++++++++++++++++++++ 公告模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 讨论模块 Start ++++++++++++++++++++++++++++++
/*
    功能：添加讨论
    传入：Json(Title, Content, ParentId, UserId) 如果是父讨论ParentId=0
    传出：Json(Result)
*/
Json::Value MoDB::InsertDiscuss(Json::Value &insertjson)
{
    Json::Value resjson;
    try
    {
        int64_t id = ++m_articleid;
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t parentid = atoll(insertjson["ParentId"].asString().data());
        int64_t userid = atoll(insertjson["UserId"].asString().data());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];
        bsoncxx::builder::stream::document document{};
        document
            << "_id" << id
            << "Title" << title.data()
            << "Content" << content.data()
            << "ParentId" << parentid
            << "UserId" << userid
            << "Views" << 0
            << "Comments" << 0
            << "CreateTime" << GetTime().data()
            << "UpdateTime" << GetTime().data();

        auto result = discusscoll.insert_one(document.view());

        if ((*result).result().inserted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：分页查询讨论
    传入：Json(SearchInfo(ParentId, UserId), Page, PageSize)
    传出：Json(Result, Reason, ArrayInfo(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName), TotalNum)
*/
Json::Value MoDB::SelectDiscussList(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t parentid = stoll(queryjson["SearchInfo"]["ParentId"].asString());
        int64_t userid = stoll(queryjson["SearchInfo"]["UserId"].asString());
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];

        if (parentid > 0)
        {
            pipe.match({make_document(kvp("ParentId", parentid))});
            pipetot.match({make_document(kvp("ParentId", parentid))});
        }

        if (userid > 0)
        {
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        if (parentid == 0 && userid == 0)
        {
            pipe.match({make_document(kvp("ParentId", 0))});
            pipetot.match({make_document(kvp("ParentId", 0))});
        }
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document
            << "from"
            << "User"
            << "localField"
            << "UserId"
            << "foreignField"
            << "_id"
            << "as"
            << "User";
        pipe.lookup(document.view());

        document.clear();
        document
            << "Title" << 1
            << "Views" << 1
            << "Comments" << 1
            << "CreateTime" << 1
            << "User.Avatar" << 1
            << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = discusscoll.aggregate(pipe);
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：管理员分页查询讨论
    传入：Json(Page, PageSize)
    传出：Json(_id, Title, Views, Comments, CreateTime, UserId, User.Avatar, User.NickName)
*/
Json::Value MoDB::SelectDiscussListByAdmin(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document
            << "from"
            << "User"
            << "localField"
            << "UserId"
            << "foreignField"
            << "_id"
            << "as"
            << "User";
        pipe.lookup(document.view());

        document.clear();
        document
            << "Title" << 1
            << "Views" << 1
            << "Comments" << 1
            << "CreateTime" << 1
            << "UserId" << 1
            << "User.Avatar" << 1
            << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = discusscoll.aggregate(pipe);
        for (auto doc : cursor)
        {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：查询讨论的详细信息，主要是编辑时的查询
    传入：Json(DiscussId)
    传出：Json(Result, Reason, Title, Content, UserId)
*/
Json::Value MoDB::SelectDiscussByEdit(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t discussid = stoll(queryjson["DiscussId"].asString());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document
            << "Title" << 1
            << "UserId" << 1
            << "Content" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        Json::Reader reader;

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！";
            return resjson;
        }
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：查询讨论的详细内容，并且将其浏览量加一
    传入：Json(DiscussId)
    传出：Json(Resutl, Reason, Title, Content, Views, Comments, CreateTime, UpdateTime, User.NickName, User.Avatar)
*/
Json::Value MoDB::SelectDiscuss(Json::Value &queryjson)
{
    Json::Value resjson;
    try
    {
        int64_t discussid = stoll(queryjson["DiscussId"].asString());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];
        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "Views" << 1 << close_document;
        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());

        // 查询Content
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document.clear();
        document
            << "from"
            << "User"
            << "localField"
            << "UserId"
            << "foreignField"
            << "_id"
            << "as"
            << "User";
        pipe.lookup(document.view());

        document.clear();
        document
            << "Title" << 1
            << "Content" << 1
            << "Views" << 1
            << "Comments" << 1
            << "CreateTime" << 1
            << "UpdateTime" << 1
            << "User._id" << 1
            << "User.Avatar" << 1
            << "User.NickName" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        if (cursor.begin() == cursor.end())
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据，可能是请求参数错误！";
            return resjson;
        }

        Json::Reader reader;
        for (auto doc : cursor)
        {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：修改讨论的评论数
    传入：Json(ArticleId, Num)
    传出：bool
*/
bool MoDB::UpdateDiscussComments(Json::Value &updatejson)
{
    try
    {
        int64_t discussid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];

        bsoncxx::builder::stream::document document{};
        document
            << "$inc" << open_document
            << "Comments" << num << close_document;
        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

/*
    功能：更新讨论
    传入：Json(DiscussId, Title, Content)
    传出；Json(Result, Reason)
*/
Json::Value MoDB::UpdateDiscuss(Json::Value &updatejson)
{
    Json::Value resjson;
    try
    {
        int64_t discussid = stoll(updatejson["DiscussId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];

        bsoncxx::builder::stream::document document{};
        document
            << "$set" << open_document
            << "Title" << title.data()
            << "Content" << content.data()
            << "UpdateTime" << GetTime().data()
            << close_document;

        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/*
    功能：删除讨论
    传入：Json(DiscussId)
    传出：Json(Result, Reason)
*/
Json::Value MoDB::DeleteDiscuss(Json::Value &deletejson)
{
    Json::Value resjson;
    try
    {
        int64_t discussid = stoll(deletejson["DiscussId"].asString());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)["OnlineJudge"]["Discuss"];

        auto result = discusscoll.delete_one({make_document(kvp("_id", discussid))});

        if ((*result).deleted_count() < 1)
        {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库删除失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    }
    catch (const std::exception &e)
    {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
// ++++++++++++++++++++++++++++++ 讨论模块 End ++++++++++++++++++++++++++++++

MoDB::MoDB()
{
    // 初始化 ID
    m_problemid = GetMaxId("Problem");
    m_statusrecordid = GetMaxId("StatusRecord");
    m_commentid = GetMaxId("Comment");
    int64_t m_announcementid = GetMaxId("Announcement");
    int64_t m_solutionid = GetMaxId("Solution");
    int64_t m_discussid = GetMaxId("Discuss");
    m_articleid = max(m_solutionid, max(m_discussid, m_announcementid));
}

MoDB::~MoDB()
{
}