#include "db/mongo_database.h"

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/pipeline.hpp>

#include "constants/db.h"
#include "constants/problem.h"
#include "utils/snowflake.hpp"  // 雪花算法

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using constants::db::COLLECTION_ANNOUNCEMENTS;
using constants::db::COLLECTION_COMMENTS;
using constants::db::COLLECTION_DISCUSSES;
using constants::db::COLLECTION_PROBLEMS;
using constants::db::COLLECTION_SOLUTIONS;
using constants::db::COLLECTION_USERS;
using constants::db::DATABASE_NAME;
namespace problem_constants = constants::problem;

using namespace std;

// 雪花算法
using snowflake_t = snowflake<1534832906275L, std::mutex>;
snowflake_t uuid;

/**
 * 功能：获取当前时间字符串
 * 返回值：时间字符串，格式为 "YYYY-MM-DD HH:MM:SS"
 */
string GetTime() {
    time_t t = time(nullptr);
    struct tm *now = localtime(&t);
    char str[50];
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", now);
    return (string)str;
}

/**
 * 功能：获取指定集合的最大 ID
 * 参数：name 集合名称
 * 返回值：最大 ID
 */
int64_t MoDB::GetMaxId(string name) {
    auto client = pool.acquire();                                       // 获取连接池中的客户端
    mongocxx::collection coll = (*client)[DATABASE_NAME][name.data()];  // 访问指定集合

    mongocxx::pipeline pipe;
    bsoncxx::builder::basic::document sort_doc{};
    sort_doc.append(kvp("_id", -1));
    pipe.sort(sort_doc.view());
    pipe.limit(1);

    mongocxx::cursor cursor = coll.aggregate(pipe);  // 执行聚合查询

    // 如果游标为空，说明集合没有数据，返回 0
    auto it = cursor.begin();
    if (it == cursor.end()) {
        return 0;
    }

    Json::Value jsonvalue;
    Json::Reader reader;
    reader.parse(bsoncxx::to_json(*it), jsonvalue);

    if (!jsonvalue["_id"].isNull()) {
        if (jsonvalue["_id"].isIntegral()) {
            return jsonvalue["_id"].asLargestInt();
        }
        if (jsonvalue["_id"].isString()) {
            return stoll(jsonvalue["_id"].asString());
        }
    }

    return 0;
}

// 局部静态特性的方式实现单实例模式
MoDB *MoDB::GetInstance() {
    static MoDB modb;
    return &modb;
}

// ------------------------------ 用户模块 Start ------------------------------
/**
 * 功能：用户注册，首先检查账户和昵称是否存在，若不存在则插入新用户
 * 传入：Json(NickName, Account, PassWord, PersonalProfile, School, Major)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::RegisterUser(Json::Value &registerjson) {
    Json::Value resjson;
    try {
        // 提取注册信息
        string account = registerjson["Account"].asString();
        string nickname = registerjson["NickName"].asString();
        string password = registerjson["PassWord"].asString();
        string personalprofile = registerjson["PersonalProfile"].asString();
        string school = registerjson["School"].asString();
        string major = registerjson["Major"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];
        mongocxx::cursor cursor = usercoll.find({make_document(kvp("Account", account.data()))});
        // 判断账户是否存在
        if (cursor.begin() != cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "账户已存在，请重新填写！";
            return resjson;
        }
        // 检查昵称是否存在
        cursor = usercoll.find({make_document(kvp("NickName", nickname.data()))});
        if (cursor.begin() != cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "昵称已存在，请重新填写！";
            return resjson;
        }

        uuid.init(1, 1);
        int64_t id = uuid.nextid();
        string jointime = GetTime();
        // 默认头像
        string avatar = "http://127.0.0.1:8081/api/image/1";
        // 插入
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Avatar" << avatar.data() << "NickName" << nickname.data() << "Account"
                 << account.data() << "PassWord" << password.data() << "PersonalProfile" << personalprofile.data()
                 << "School" << school.data() << "Major" << major.data() << "JoinTime" << jointime.data()
                 << "CommentLikes" << open_array << close_array << "Solves" << open_array << close_array << "SubmitNum"
                 << 0 << "ACNum" << 0 << "Authority" << 3;

        auto result = usercoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["Reason"] = "注册成功！";
        resjson["_id"] = (Json::Int64)id;
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 用户登录：将账号和密码作为查询条件，如果未查询出数据，则代表账号密码错误或没有该账户，如果查询出数据，直接返回即可
 * 传入：Json(Account, PassWord)
 * 传出：Json(Result, Reason, Info(_id, NickName, Avatar, CommentLikes, Solves, Authority))
 */
Json::Value MoDB::LoginUser(Json::Value &loginjson) {
    Json::Value resjson;
    try {
        // 提取登录信息
        string account = loginjson["Account"].asString();
        string password = loginjson["PassWord"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        document << "Account" << account.data() << "PassWord" << password.data();
        pipe.match(document.view());

        document.clear();
        document << "Avatar" << 1 << "NickName" << 1 << "CommentLikes" << 1 << "Solves" << 1 << "Authority" << 1;

        pipe.project(document.view());
        // 匹配账号和密码
        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        // 匹配失败
        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "账户或密码错误！";
            return resjson;
        }
        // 匹配成功
        Json::Reader reader;
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["Info"] = jsonvalue;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询用户信息
 * 传入：Json(UserId)
 * 传出：Json(Result, Reason, _id, Avatar, NickName, PersonalProfile, School, Major, JoinTime, Solves, ACNum,
 * SubmitNum)
 */
Json::Value MoDB::SelectUserInfo(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取用户ID
        int64_t userid = stoll(queryjson["UserId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;

        pipe.match({make_document(kvp("_id", userid))});

        document << "Avatar" << 1 << "NickName" << 1 << "PersonalProfile" << 1 << "School" << 1 << "Major" << 1
                 << "JoinTime" << 1 << "Solves" << 1 << "ACNum" << 1 << "SubmitNum" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询用户信息（在设置页面修改用户时使用）
 * 传入：Json(UserId)
 * 传出：Json(_id, Avatar, NickName, PersonalProfile, School, Major)
 */
Json::Value MoDB::SelectUserUpdateInfo(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取用户 ID
        int64_t userid = stoll(queryjson["UserId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;

        pipe.match({make_document(kvp("_id", userid))});
        document << "Avatar" << 1 << "NickName" << 1 << "PersonalProfile" << 1 << "School" << 1 << "Major" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新用户信息
 * 传入：Json(UserId, Avatar, NickName, PersonalProfile, School, Major)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::UpdateUserInfo(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        // 获取用户ID和更新信息
        int64_t userid = stoll(updatejson["UserId"].asString());
        string avatar = updatejson["Avatar"].asString();
        string nickname = updatejson["NickName"].asString();
        string personalprofile = updatejson["PersonalProfile"].asString();
        string school = updatejson["School"].asString();
        string major = updatejson["Major"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Avatar" << avatar.data() << "NickName" << nickname.data()
                 << "PersonalProfile" << personalprofile.data() << "School" << school.data() << "Major" << major.data()
                 << close_document;

        usercoll.update_one({make_document(kvp("_id", userid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除用户
 * 传入：Json(UserId)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::DeleteUser(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        // 提取用户 ID
        int64_t userid = stoll(deletejson["UserId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        auto result = usercoll.delete_one({make_document(kvp("_id", userid))});

        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：用户排名查询
 * 传入：Json(Page, PageSize)
 * 传出：Json(ArrayInfo[_id, Rank, Avatar, NickName, PersonalProfile, SubmitNum, ACNum], TotalNum)
 */
Json::Value MoDB::SelectUserRank(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;
        Json::Reader reader;

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = usercoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        // ACNum 值越大，越靠前；若 ACNum 相同，则 SubmitNum 越小排名越高
        document << "ACNum" << -1 << "SubmitNum" << 1;

        pipe.sort(document.view());
        pipe.skip(skip);
        pipe.limit(pagesize);
        document.clear();
        document << "Avatar" << 1 << "NickName" << 1 << "PersonalProfile" << 1 << "SubmitNum" << 1 << "ACNum" << 1;
        pipe.project(document.view());

        cursor = usercoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        // 添加Rank排名
        int currank = (page - 1) * pagesize + 1;
        for (int i = 0; i < resjson["ArrayInfo"].size(); i++) {
            resjson["ArrayInfo"][i]["Rank"] = currank++;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询用户列表（管理员权限）
 * 传入：Json(Page, PageSize)
 * Json(Result, Reason, ArrayInfo[_id, NickName, PersonalProfile, School, Major, JoinTime], TotalNum)
 */
Json::Value MoDB::SelectUserSetInfo(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.skip(skip);
        pipe.limit(pagesize);

        document << "NickName" << 1 << "PersonalProfile" << 1 << "School" << 1 << "Major" << 1 << "JoinTime" << 1;

        pipe.project(document.view());
        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);

        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        resjson["TotalNum"] = to_string(usercoll.count_documents({}));
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新用户题目信息（用于用户提交代码后更新题目完成情况）
 * 传入：Json(UserId, ProblemId, Status)
 * 传出：bool(如果题目第一次 AC 则返回 true，否则返回 false)
 */
bool MoDB::UpdateUserProblemInfo(Json::Value &updatejson) {
    try {
        int64_t userid = stoll(updatejson["UserId"].asString());
        int problemid = stoi(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        // 将用户提交数目加一
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "SubmitNum" << 1 << close_document;

        usercoll.update_one({make_document(kvp("_id", userid))}, document.view());

        // 如果题目 AC 了
        if (status == 2) {
            // 查询 AC 题目是否已经添加至 Solves 的数组中
            mongocxx::pipeline pipe;
            pipe.match({make_document(kvp("_id", userid))});
            document.clear();
            document << "IsHasAc" << open_document << "$in" << open_array << problemid << "$Solves" << close_array
                     << close_document;
            pipe.project(document.view());
            Json::Reader reader;
            Json::Value tmpjson;
            mongocxx::cursor cursor = usercoll.aggregate(pipe);

            for (auto doc : cursor) {
                reader.parse(bsoncxx::to_json(doc), tmpjson);
            }
            // 如果未添加
            if (tmpjson["IsHasAc"].asBool() == false) {
                document.clear();
                document << "$push" << open_document << "Solves" << problemid << close_document << "$inc"
                         << open_document << "ACNum" << 1 << close_document;
                usercoll.update_one({make_document(kvp("_id", userid))}, document.view());
                return true;
            }
            return false;
        }
        return false;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
/**
 * 功能：查询题目信息（单条）
 * 传入：Json(ProblemId)
 * Json(Result, Reason, _id, Title,Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum, ACNum, UserNickName,
 * Tags)
 */
Json::Value MoDB::SelectProblem(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取题目 ID
        int64_t problemid = stoll(queryjson["ProblemId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", problemid))});

        document << "Title" << 1 << "Description" << 1 << "TimeLimit" << 1 << "MemoryLimit" << 1 << "JudgeNum" << 1
                 << "SubmitNum" << 1 << "ACNum" << 1 << "UserNickName" << 1 << "Tags" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = problemcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}

/**
 * 功能：查询题目信息（管理员权限）
 * 传入：Json(ProblemId)
 * 传出：Json(Result, Reason,_id, Title, Description, TimeLimit, MemoryLimit, UserNickName, JudgeNum, Tags)
 */
Json::Value MoDB::SelectProblemInfoByAdmin(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取题目 ID
        int64_t problemid = stoll(queryjson["ProblemId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", problemid))});

        document << "Title" << 1 << "Description" << 1 << "TimeLimit" << 1 << "MemoryLimit" << 1 << "JudgeNum" << 1
                 << "UserNickName" << 1 << "Tags" << 1;
        pipe.project(document.view());

        Json::Reader reader;

        mongocxx::cursor cursor = problemcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该信息！";
            return resjson;
        }

        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：插入题目（管理员权限）
 * 传入：Json(Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
 * 传出：Json(Result, Reason, ProblemId)
 */
Json::Value MoDB::InsertProblem(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        // 构造题目 ID（题目集合中 ID 的最大值 + 1）
        int64_t problemid = ++m_problem_id;
        string title = insertjson["Title"].asString();
        string description = insertjson["Description"].asString();
        int timelimit = stoi(insertjson["TimeLimit"].asString());
        int memorylimit = stoi(insertjson["MemoryLimit"].asString());
        int judgenum = stoi(insertjson["JudgeNum"].asString());
        string usernickname = insertjson["UserNickName"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        bsoncxx::builder::stream::document document{};
        auto in_array = document << "_id" << problemid << "Title" << title.data() << "Description" << description.data()
                                 << "TimeLimit" << timelimit << "MemoryLimit" << memorylimit << "JudgeNum" << judgenum
                                 << "SubmitNum" << 0 << "CENum" << 0 << "ACNum" << 0 << "WANum" << 0 << "RENum" << 0
                                 << "TLENum" << 0 << "MLENum" << 0 << "SENum" << 0 << "UserNickName"
                                 << usernickname.data() << "Tags" << open_array;
        for (int i = 0; i < insertjson["Tags"].size(); i++) {
            string tag = insertjson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << finalize;
        auto result = problemcoll.insert_one(doc.view());

        if ((*result).result().inserted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["ProblemId"] = (Json::Int64)problemid;
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新题目信息（管理员权限）
 * 传入：Json(ProblemId, Title, Description, TimeLimit, MemoryLimit, JudgeNum, Tags, UseNickName)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::UpdateProblem(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        // 提取题目信息
        int problemid = stoi(updatejson["ProblemId"].asString());
        string title = updatejson["Title"].asString();
        string description = updatejson["Description"].asString();
        int timelimit = stoi(updatejson["TimeLimit"].asString());
        int memorylimit = stoi(updatejson["MemoryLimit"].asString());
        int judgenum = stoi(updatejson["JudgeNum"].asString());
        string usernickname = updatejson["UserNickName"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        bsoncxx::builder::stream::document document{};
        auto in_array = document << "$set" << open_document << "Title" << title.data() << "Description"
                                 << description.data() << "TimeLimit" << timelimit << "MemoryLimit" << memorylimit
                                 << "JudgeNum" << judgenum << "UserNickName" << usernickname.data() << "Tags"
                                 << open_array;
        for (int i = 0; i < updatejson["Tags"].size(); i++) {
            string tag = updatejson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << close_document << finalize;

        problemcoll.update_one({make_document(kvp("_id", problemid))}, doc.view());

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除题目（管理员权限）
 * 传入：Json(ProblemId)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::DeleteProblem(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        // 提取题目 ID
        int64_t problemid = stoll(deletejson["ProblemId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        auto result = problemcoll.delete_one({make_document(kvp("_id", problemid))});

        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：将字符串转换为整数（仅限正整数，且长度小于4）
 */
int mystoi(string num) {
    int resnum = 0;
    if (num.size() >= 4 || num.size() == 0)
        return resnum;
    for (auto n : num) {
        if (isdigit(n)) {
            resnum = resnum * 10 + n - '0';
        }
    }
    return resnum;
}

/**
 * 功能：分页获取题目列表
 * 传入：Json(Page, PageSize, SearchInfo{Id, Title, Tags[]})
 * 传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum,
 * TLENum, MLENum, SENum, Tags]), TotalNum)
 */
Json::Value MoDB::SelectProblemList(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取查询信息
        Json::Value searchinfo = queryjson["SearchInfo"];
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];
        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 查询 ID
        if (searchinfo["Id"].asString().size() > 0) {
            int id = mystoi(searchinfo["Id"].asString());
            pipe.match({make_document(kvp("_id", id))});
            pipetot.match({make_document(kvp("_id", id))});
        }

        // 查询标题
        if (searchinfo["Title"].asString().size() > 0) {
            document << "Title" << open_document << "$regex" << searchinfo["Title"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询标签
        if (searchinfo["Tags"].size() > 0) {
            auto in_array = document << "Tags" << open_document << "$in" << open_array;
            for (int i = 0; i < searchinfo["Tags"].size(); i++) {
                in_array = in_array << searchinfo["Tags"][i].asString();
            }
            bsoncxx::document::value doc = in_array << close_array << close_document << finalize;
            pipe.match(doc.view());
            pipetot.match(doc.view());
            document.clear();
        }
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行
        document << "ProblemId" << "$_id" << "Title" << 1 << "SubmitNum" << 1 << "CENum" << 1 << "ACNum" << 1 << "WANum"
                 << 1 << "RENum" << 1 << "TLENum" << 1 << "MLENum" << 1 << "SENum" << 1 << "Tags" << 1;
        pipe.project(document.view());

        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Reuslt"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}

/**
 * 功能：分页获取题目列表（管理员权限）
 * 传入：Json(Page, PageSize)
 * 传出：Json((Result, Reason, ArrayInfo[ProblemId, Title, SubmitNum, CENum, ACNum, WANum, RENum,
 * TLENum, MLENum, SENum, Tags]), TotalNum)
 */
Json::Value MoDB::SelectProblemListByAdmin(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取查询信息
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行
        document << "ProblemId" << "$_id" << "Title" << 1 << "SubmitNum" << 1 << "CENum" << 1 << "ACNum" << 1 << "WANum"
                 << 1 << "RENum" << 1 << "TLENum" << 1 << "MLENum" << 1 << "SENum" << 1 << "Tags" << 1;
        pipe.project(document.view());

        Json::Value arryjson;
        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            arryjson.append(jsonvalue);
        }
        resjson["ArrayInfo"] = arryjson;
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库出错啦！";
        return resjson;
    }
}

/**
 * 功能：更新题目的状态数量
 * 传入：Json(ProblemId, Status)
 * 传出：bool
 */
bool MoDB::UpdateProblemStatusNum(Json::Value &updatejson) {
    try {
        int64_t problemid = stoll(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        const char *status_field = nullptr;
        if (status == problem_constants::STATUS_COMPILE_ERROR)
            status_field = problem_constants::FIELD_COMPILE_ERROR;
        else if (status == problem_constants::STATUS_ACCEPTED)
            status_field = problem_constants::FIELD_ACCEPTED;
        else if (status == problem_constants::STATUS_WRONG_ANSWER)
            status_field = problem_constants::FIELD_WRONG_ANSWER;
        else if (status == problem_constants::STATUS_RUNTIME_ERROR)
            status_field = problem_constants::FIELD_RUNTIME_ERROR;
        else if (status == problem_constants::STATUS_TIME_LIMIT_EXCEEDED)
            status_field = problem_constants::FIELD_TIME_LIMIT_EXCEEDED;
        else if (status == problem_constants::STATUS_MEMORY_LIMIT_EXCEEDED)
            status_field = problem_constants::FIELD_MEMORY_LIMIT_EXCEEDED;
        else if (status == problem_constants::STATUS_SYSTEM_ERROR)
            status_field = problem_constants::FIELD_SYSTEM_ERROR;

        if (status_field == nullptr)
            return false;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        const std::string status_field_name(status_field);
        auto update_doc = make_document(kvp("$inc", make_document(kvp("SubmitNum", 1), kvp(status_field_name, 1))));

        problemcoll.update_one({make_document(kvp("_id", problemid))}, update_doc.view());
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 题目模块 End ------------------------------

// ------------------------------ 标签模块 Start ------------------------------
/**
 * 功能：获取题目的所有标签
 * 传入：void
 * 传出：Json(Tags)
 */
Json::Value MoDB::GetProblemTags() {
    // 获取数据库连接
    auto client = pool.acquire();
    mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];
    mongocxx::cursor cursor = problemcoll.distinct({"Tags"}, {});
    Json::Reader reader;
    Json::Value resjson;
    for (auto doc : cursor) {
        reader.parse(bsoncxx::to_json(doc), resjson);
    }
    return resjson;
}
// ------------------------------ 标签模块 End ------------------------------

// ------------------------------ 公告模块 Start ------------------------------
/**
 * 功能：添加公告（管理员权限）
 * 传入：Json(Title, Content, UserId, Level)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::InsertAnnouncement(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        // 构造公告 ID（公告集合中 ID 的最大值 + 1）
        int64_t id = ++m_announcement_id;
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        int level = stoi(insertjson["Level"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "UserId" << userid
                 << "Views" << 0 << "Comments" << 0 << "Level" << level << "CreateTime" << GetTime().data()
                 << "UpdateTime" << GetTime().data();

        auto result = announcementcoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询公告详细信息，并将其浏览量加 1
 * 传入：Json(AnnouncementId)
 * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime], TotalNum)
 */
Json::Value MoDB::SelectAnnouncement(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取公告 ID
        int64_t announcementid = stoll(queryjson["AnnouncementId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Views" << 1 << close_document;
        announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());

        // 查询
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", announcementid))});
        document.clear();
        document << "Title" << 1 << "Content" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "UpdateTime" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = announcementcoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据，可能是请求参数出错！";
            return resjson;
        }

        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新公告（管理员权限）
 * 传入：Json(AnnouncementId, Title, Content, Level)
 * 传出；Json(Result, Reason)
 */
Json::Value MoDB::UpdateAnnouncement(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        // 提取公告信息
        int64_t announcementid = stoll(updatejson["AnnouncementId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();
        int level = stoi(updatejson["Level"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "Level"
                 << level << "UpdateTime" << GetTime().data() << close_document;

        announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除公告（管理员权限）
 * 传入：Json(AnnouncementId)
 * 传出：Json(Result,Reason)
 */
Json::Value MoDB::DeleteAnnouncement(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        // 提取公告 ID
        int64_t announcementid = stoll(deletejson["AnnouncementId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        auto result = announcementcoll.delete_one({make_document(kvp("_id", announcementid))});
        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库删除失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询公告列表
 * 传入：Json(Page, PageSize)
 * 传出：Json([Result, Reason, _id, Title, Views, Comments, CreateTime], TotalNum)
 */
Json::Value MoDB::SelectAnnouncementList(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取查询信息
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = announcementcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.sort({make_document(kvp("Level", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);

        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1;
        pipe.project(document.view());

        cursor = announcementcoll.aggregate(pipe);

        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询公告的详细信息，主要是编辑时的查询
 * 传入：Json(AnnouncementId)
 * 传出：Json(Result, Reason, Title, Content, Level)
 */
Json::Value MoDB::SelectAnnouncementByEdit(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取公告 ID
        int64_t announcementid = stoll(queryjson["AnnouncementId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", announcementid))});
        document << "Title" << 1 << "Content" << 1 << "Level" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = announcementcoll.aggregate(pipe);

        Json::Reader reader;
        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！,可能是请求参数出错！";
            return resjson;
        }
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新公告的评论数量
 * 传入：Json(ArticleId, Num)
 * 传出：bool
 */
bool MoDB::UpdateAnnouncementComments(Json::Value &updatejson) {
    try {
        int64_t articleid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;
        announcementcoll.update_one({make_document(kvp("_id", articleid))}, document.view());
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
/**
 * 功能：添加讨论
 * 传入：Json(Title, Content, ParentId, UserId) 如果是父讨论ParentId=0
 * 传出：Json(Result)
 */
Json::Value MoDB::InsertDiscuss(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        // 构造讨论 ID（讨论集合中 ID 的最大值 + 1）
        int64_t id = ++m_discussion_id;
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t parentid = atoll(insertjson["ParentId"].asString().data());
        int64_t userid = atoll(insertjson["UserId"].asString().data());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "ParentId" << parentid
                 << "UserId" << userid << "Views" << 0 << "Comments" << 0 << "CreateTime" << GetTime().data()
                 << "UpdateTime" << GetTime().data();

        auto result = discusscoll.insert_one(document.view());

        if ((*result).result().inserted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询讨论的详细内容，并且将其浏览量加 1
 * 传入：Json(DiscussId)
 * 传出：Json(Result, Reason, _id, Title, Content, Views, Comments, CreateTime, UpdateTime, User.NickName,
 * User.Avatar)
 */
Json::Value MoDB::SelectDiscuss(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        // 提取讨论 ID
        int64_t discussid = stoll(queryjson["DiscussId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];
        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Views" << 1 << close_document;
        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());

        // 查询Content
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document.clear();
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Content" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "UpdateTime" << 1 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据，可能是请求参数错误！";
            return resjson;
        }

        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新讨论
 * 传入：Json(DiscussId, Title, Content)
 * 传出；Json(Result,Reason)
 */
Json::Value MoDB::UpdateDiscuss(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        int64_t discussid = stoll(updatejson["DiscussId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "UpdateTime"
                 << GetTime().data() << close_document;

        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除讨论
 * 传入：Json(DiscussId)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::DeleteDiscuss(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        // 提取讨论 ID
        int64_t discussid = stoll(deletejson["DiscussId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        auto result = discusscoll.delete_one({make_document(kvp("_id", discussid))});

        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库删除失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询讨论
 * 传入：Json(SearchInfo, Page, PageSize)
 * 传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
 */
Json::Value MoDB::SelectDiscussList(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t parentid = stoll(queryjson["SearchInfo"]["ParentId"].asString());
        int64_t userid = stoll(queryjson["SearchInfo"]["UserId"].asString());
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        if (parentid > 0) {
            pipe.match({make_document(kvp("ParentId", parentid))});
            pipetot.match({make_document(kvp("ParentId", parentid))});
        }

        if (userid > 0) {
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        if (parentid == 0 && userid == 0) {
            pipe.match({make_document(kvp("ParentId", 0))});
            pipetot.match({make_document(kvp("ParentId", 0))});
        }
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "User.Avatar" << 1
                 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = discusscoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询讨论（管理员权限）
 * 传入：Json(Page,PageSize)
 * 传出：Json(_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName)
 */
Json::Value MoDB::SelectDiscussListByAdmin(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];
        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "UserId" << 1
                 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = discusscoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询讨论的详细信息，主要是编辑时的查询
 * 传入：Json(DiscussId)
 * 传出：Json(Result, Reason, Title, Content)
 */
Json::Value MoDB::SelectDiscussByEdit(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t discussid = stoll(queryjson["DiscussId"].asString());

        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document << "Title" << 1 << "UserId" << 1 << "Content" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        Json::Reader reader;

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！";
            return resjson;
        }
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新讨论的评论数量
 * 传入：Json(DiscussId, Num)
 * 传出：bool
 */
bool MoDB::UpdateDiscussComments(Json::Value &updatejson) {
    try {
        int64_t discussid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;
        discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
/**
 * 功能：添加题解
 * 传入：Json(Title, Content, ParentId, UserId, Public)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::InsertSolution(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        // 构造题解 ID（题解集合中 ID 的最大值 + 1）
        int64_t id = ++m_solution_id;
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t parentid = stoll(insertjson["ParentId"].asString());
        int64_t userid = stoll(insertjson["UserId"].asString());
        bool ispublic = insertjson["Public"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "ParentId" << parentid
                 << "UserId" << userid << "Views" << 0 << "Comments" << 0 << "Public" << ispublic << "CreateTime"
                 << GetTime().data() << "UpdateTime" << GetTime().data();

        auto result = solutioncoll.insert_one(document.view());

        if ((*result).result().inserted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库插入失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询题解的详细内容，并且将其浏览量加 1
 * 传入：Json(SolutionId)
 * 传出：Json(Result, Reason, Title, Content, Views, Comments, CreateTime, UpdateTime, User.NickName, User.Avatar)
 */
Json::Value MoDB::SelectSolution(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t solutionid = stoll(queryjson["SolutionId"].asString());

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];
        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Views" << 1 << close_document;
        solutioncoll.update_one({make_document(kvp("_id", solutionid))}, document.view());

        // 查询Content
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", solutionid))});
        document.clear();

        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Content" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "UpdateTime" << 1 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        mongocxx::cursor cursor = solutioncoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据，可能是请求参数错误！";
            return resjson;
        }
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：更新题解
 * 传入：Json(SolutionId, Title, Content, Public)
 * 传出；Json(Result, Reason)
 */
Json::Value MoDB::UpdateSolution(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        int64_t solutionid = stoll(updatejson["SolutionId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();
        bool ispublic = updatejson["Public"].asBool();

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "Public"
                 << ispublic << "UpdateTime" << GetTime().data() << close_document;

        solutioncoll.update_one({make_document(kvp("_id", solutionid))}, document.view());

        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除题解
 * 传入：Json(SolutionId)
 * 传出：Json(Result, Reason)
 */
Json::Value MoDB::DeleteSolution(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        int64_t solutionid = stoll(deletejson["SolutionId"].asString());

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        auto result = solutioncoll.delete_one({make_document(kvp("_id", solutionid))});

        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库删除失败！";
            return resjson;
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询题解（公开题解）
 * 传入：Json(SearchInfo(ParentId, UserId), Page, PageSize)
 * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName], TotalNum)
 */
Json::Value MoDB::SelectSolutionList(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t parentid = stoll(queryjson["SearchInfo"]["ParentId"].asString());
        int64_t userid = stoll(queryjson["SearchInfo"]["UserId"].asString());
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        if (parentid > 0) {
            // 匹配ID
            pipetot.match({make_document(kvp("ParentId", parentid))});
            pipe.match({make_document(kvp("ParentId", parentid))});

            // 匹配公开
            pipetot.match({make_document(kvp("Public", true))});
            pipe.match({make_document(kvp("Public", true))});
        }
        if (userid > 0) {
            // 匹配ID
            pipetot.match({make_document(kvp("UserId", userid))});
            pipe.match({make_document(kvp("UserId", userid))});
        }
        // 获取总条数

        pipetot.count("TotalNum");
        mongocxx::cursor cursor = solutioncoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "User.Avatar" << 1
                 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = solutioncoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：分页查询题解（管理员权限）
 * 传入：Json(Page, PageSize)
 * 传出：Json(Result, Reason, ArrayInfo[_id, Title, Views, Comments, CreateTime, User.Avatar, User.NickName], TotalNum)
 */
Json::Value MoDB::SelectSolutionListByAdmin(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 获取总条数
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = solutioncoll.aggregate(pipetot);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "UserId" << 1
                 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = solutioncoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：查询题解的详细信息，主要是编辑时的查询
 * 传入：Json(SolutionId)
 * 传出：Json(Result, Reason, Title, Content, UserId, Public)
 */
Json::Value MoDB::SelectSolutionByEdit(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t solutionid = stoll(queryjson["SolutionId"].asString());

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", solutionid))});
        document << "Title" << 1 << "Content" << 1 << "UserId" << 1 << "Public" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = solutioncoll.aggregate(pipe);

        Json::Reader reader;
        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！";
            return resjson;
        }
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：修改题解的评论数
 * 传入：Json(ArticleId, Num)
 * 传出：bool
 */
bool MoDB::UpdateSolutionComments(Json::Value &updatejson) {
    try {
        int64_t articleid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;
        solutioncoll.update_one({make_document(kvp("_id", articleid))}, document.view());
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 题解模块 End ------------------------------

// ------------------------------ 评论模块 Start ------------------------------
/**
 * 功能：管理员查询评论
 * 传入：Json(Page, PageSize)
 * 传出：Json(_id, ParentId, ParentType, Content, CreateTime, Child_Comments._id, Child_Comments.Content,
 * Child_Comments.CreateTime)
 */
Json::Value MoDB::SelectCommentListByAdmin(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        // 按照时间先后顺序
        pipe.sort({make_document(kvp("CreateTime", -1))});
        // 跳过多少条
        pipe.skip(skip);
        // 限制多少条
        pipe.limit(pagesize);

        // 选择需要的字段
        document << "ParentId" << 1 << "ParentType" << 1 << "Content" << 1 << "CreateTime" << 1 << "Child_Comments._id"
                 << 1 << "Child_Comments.Content" << 1 << "Child_Comments.CreateTime" << 1;
        pipe.project(document.view());
        document.clear();

        Json::Reader reader;

        mongocxx::cursor cursor = commentcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArrayInfo"].append(jsonvalue);
        }
        resjson["TotalNum"] = to_string(commentcoll.count_documents({}));
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
/**
 * 功能：查询父评论
 * 传入：Json(ParentId, Skip, Limie, SonNum)
 * 传出：Json(ParentId, Content, Likes, CreateTime, Child_Total, User(Avatar, NickName),
 * Child_Comments(_id, Content, Likes, CreateTime, User(Avatar, NickName)))
 */
Json::Value MoDB::getFatherComment(Json::Value &queryjson) {
    Json::Value resjson;
    try {
        int64_t parentid = stoll(queryjson["ParentId"].asString());
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int sonnum = stoi(queryjson["SonNum"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        // 匹配ParentId
        pipe.match({make_document(kvp("ParentId", parentid))});
        // 按照时间先后顺序
        pipe.sort({make_document(kvp("CreateTime", 1))});
        // 跳过多少条
        pipe.skip(skip);
        // 限制多少条
        pipe.limit(pagesize);
        // 将子评论进行求个数
        document << "$set" << open_document << "Child_Total" << open_document << "$size" << "$Child_Comments"
                 << close_document << close_document;

        pipe.append_stage(document.view());
        document.clear();
        // 限制子评论的个数
        document << "$set" << open_document << "Child_Comments" << open_document << "$slice" << open_array
                 << "$Child_Comments" << 0 << sonnum << close_array << close_document << close_document;

        pipe.append_stage(document.view());
        document.clear();
        // 将数组拆散
        document << "path" << "$Child_Comments" << "preserveNullAndEmptyArrays" << true;
        pipe.unwind(document.view());
        document.clear();
        // 将子评论的用户id和用户表进行外连接
        document << "from" << "User" << "localField" << "Child_Comments.UserId" << "foreignField" << "_id" << "as"
                 << "Child_Comments.User";
        pipe.lookup(document.view());
        document.clear();
        // 将其合并
        document << "_id" << "$_id" << "ParentId" << open_document << "$first" << "$ParentId" << close_document
                 << "Content" << open_document << "$first" << "$Content" << close_document << "Likes" << open_document
                 << "$first" << "$Likes" << close_document << "UserId" << open_document << "$first" << "$UserId"
                 << close_document << "CreateTime" << open_document << "$first" << "$CreateTime" << close_document
                 << "Child_Total" << open_document << "$first" << "$Child_Total" << close_document << "Child_Comments"
                 << open_document << "$push" << "$Child_Comments" << close_document;
        pipe.group(document.view());
        document.clear();
        // 将父评论的用户Id和用户表进行外连接
        document << "from" << "User" << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());
        document.clear();
        // 选择需要的字段
        document << "ParentId" << 1 << "Content" << 1 << "Likes" << 1 << "CreateTime" << 1 << "Child_Total" << 1
                 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1 << "Child_Comments._id" << 1
                 << "Child_Comments.Content" << 1 << "Child_Comments.Likes" << 1 << "Child_Comments.CreateTime" << 1
                 << "Child_Comments.User._id" << 1 << "Child_Comments.User.Avatar" << 1
                 << "Child_Comments.User.NickName" << 1;
        pipe.project(document.view());
        document.clear();
        // 按照时间先后顺序
        pipe.sort({make_document(kvp("CreateTime", 1))});

        Json::Reader reader;

        mongocxx::cursor cursor = commentcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            resjson["ArryInfo"].append(jsonvalue);
        }
        resjson["TotalNum"] = to_string(commentcoll.count_documents({make_document(kvp("ParentId", parentid))}));
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：获取子评论
 * 传入：Json(ParentId, Skip, Limit)
 * 传出：Json(Child_Total, Child_Comments(_id, Content, Likes, CreateTime, User(NickName, Avatar)))
 */
Json::Value MoDB::getSonComment(Json::Value &queryjson) {
    Json::Value resjson;

    try {
        int64_t _id = stoll(queryjson["ParentId"].asString());
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        // 匹配id
        pipe.match({make_document(kvp("_id", _id))});

        // 将子评论进行求个数
        document << "$set" << open_document << "Child_Total" << open_document << "$size" << "$Child_Comments"
                 << close_document << close_document;

        pipe.append_stage(document.view());
        document.clear();
        // 限制子评论个数
        document << "$set" << open_document << "Child_Comments" << open_document << "$slice" << open_array
                 << "$Child_Comments" << skip << pagesize << close_array << close_document << close_document;

        pipe.append_stage(document.view());
        document.clear();
        // 将数组拆散
        document << "path" << "$Child_Comments" << "preserveNullAndEmptyArrays" << true;
        pipe.unwind(document.view());
        document.clear();
        // 将子评论的用户id和用户表进行外连接
        document << "from" << "User" << "localField" << "Child_Comments.UserId" << "foreignField" << "_id" << "as"
                 << "Child_Comments.User";
        pipe.lookup(document.view());
        document.clear();

        // 将其合并
        document << "_id" << "$_id" << "Child_Total" << open_document << "$first" << "$Child_Total" << close_document
                 << "Child_Comments" << open_document << "$push" << "$Child_Comments" << close_document;
        pipe.group(document.view());
        document.clear();
        document << "Child_Total" << 1 << "Child_Comments._id" << 1 << "Child_Comments.Content" << 1
                 << "Child_Comments.Likes" << 1 << "Child_Comments.CreateTime" << 1 << "Child_Comments.User._id" << 1
                 << "Child_Comments.User.NickName" << 1 << "Child_Comments.User.Avatar" << 1;
        pipe.project(document.view());
        document.clear();

        Json::Reader reader;

        mongocxx::cursor cursor = commentcoll.aggregate(pipe);
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), resjson);
        }
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：插入父评论
 * 传入：Json(ParentId, Content, UserId)
 * 传出：Json(_id, CreateTime)
 */
Json::Value MoDB::InsertFatherComment(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        int64_t id = ++m_comment_id;
        int64_t parentid = stoll(insertjson["ParentId"].asString());
        string parenttype = insertjson["ArticleType"].asString();
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        string createtime = GetTime();

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "ParentId" << parentid << "ParentType" << parenttype.data() << "Content"
                 << content.data() << "UserId" << userid << "Likes" << 0 << "CreateTime" << createtime.data()
                 << "Child_Comments" << open_array << close_array;

        commentcoll.insert_one(document.view());
        resjson["_id"] = to_string(id);
        resjson["CreateTime"] = createtime.data();
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：插入子评论
 * 传入：Json(ParentId, Content, UserId)
 * 传出：Json(_id, CreateTime)
 */
Json::Value MoDB::InsertSonComment(Json::Value &insertjson) {
    Json::Value resjson;
    try {
        int64_t parentid = stoll(insertjson["ParentId"].asString().data());
        int64_t id = ++m_comment_id;
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString().data());
        string createtime = GetTime();

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        bsoncxx::builder::stream::document document{};
        document << "$addToSet" << open_document << "Child_Comments" << open_document << "_id" << id << "Content"
                 << content.data() << "UserId" << userid << "Likes" << 0 << "CreateTime" << createtime.data()
                 << close_document << close_document;

        commentcoll.update_one({make_document(kvp("_id", parentid))}, document.view());

        resjson["_id"] = to_string(id);
        resjson["CreateTime"] = createtime;
        resjson["Result"] = "Success";
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除某一篇文章（讨论，题解，公告）的所有文章，主要服务于删除文章
 * 传入：Json(ArticleId)
 * 传出：bool
 */
bool MoDB::DeleteArticleComment(Json::Value &deletejson) {
    try {
        int64_t articleid = stoll(deletejson["ArticleId"].asString());

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        auto result = commentcoll.delete_many({make_document(kvp("ParentId", articleid))});

        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

/**
 * 功能：删除父评论
 * 传入：Json(CommentId)
 * 传出：Json(Result, Reason, DeleteNum)
 */
Json::Value MoDB::DeleteFatherComment(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        int64_t commentid = stoll(deletejson["CommentId"].asString());

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        mongocxx::cursor cursor = commentcoll.find({make_document(kvp("_id", commentid))});
        // 如果未查询到父评论
        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该评论！";
            return resjson;
        }

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};

        pipe.match({make_document(kvp("_id", commentid))});
        document << "$set" << open_document << "Child_Total" << open_document << "$size" << "$Child_Comments"
                 << close_document << close_document;

        pipe.append_stage(document.view());
        cursor = commentcoll.aggregate(pipe);

        Json::Value jsonvalue;
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
        }
        int sonnum = stoi(jsonvalue["Child_Total"].asString());
        string articletype = jsonvalue["ParentType"].asString();

        auto result = commentcoll.delete_one({make_document(kvp("_id", commentid))});

        if ((*result).deleted_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["DeleteNum"] = sonnum + 1;
        resjson["ArticleType"] = articletype;
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}

/**
 * 功能：删除子评论
 * 传入：Json(CommentId)
 * 传出：Json(Result, Reason, DeleteNum)
 */
Json::Value MoDB::DeleteSonComment(Json::Value &deletejson) {
    Json::Value resjson;
    try {
        int64_t commentid = stoll(deletejson["CommentId"].asString());

        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        // 找出父评论ID
        mongocxx::cursor cursor = commentcoll.find({make_document(kvp("Child_Comments._id", commentid))});
        Json::Value jsonvalue;
        Json::Reader reader;

        if (cursor.begin() == cursor.end()) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未查询到数据！";
            return resjson;
        }
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
        }
        int64_t fatherid = stoll(jsonvalue["_id"].asString());
        string articletype = jsonvalue["ParentType"].asString();
        // 删除子评论
        bsoncxx::builder::stream::document document{};
        document << "$pull" << open_document << "Child_Comments" << open_document << "_id" << commentid
                 << close_document << close_document;

        auto result = commentcoll.update_one({make_document(kvp("_id", fatherid))}, document.view());

        if ((*result).matched_count() < 1) {
            resjson["Result"] = "Fail";
            resjson["Reason"] = "数据库未找到该数据！";
            return resjson;
        }
        resjson["Result"] = "Success";
        resjson["DeleteNum"] = 1;
        resjson["ArticleType"] = articletype;
        return resjson;
    } catch (const std::exception &e) {
        resjson["Result"] = "500";
        resjson["Reason"] = "数据库异常！";
        return resjson;
    }
}
// ------------------------------ 评论模块 End ------------------------------

MoDB::MoDB() {
    // 构造函数实现

    // 初始化 ID
    m_problem_id = GetMaxId(constants::db::COLLECTION_PROBLEMS);
    m_status_record_id = GetMaxId(constants::db::COLLECTION_STATUS_RECORDS);
    m_comment_id = GetMaxId(constants::db::COLLECTION_COMMENTS);
    int64_t m_announcement_id = GetMaxId(constants::db::COLLECTION_ANNOUNCEMENTS);
    int64_t m_solution_id = GetMaxId(constants::db::COLLECTION_SOLUTIONS);
    int64_t m_discussion_id = GetMaxId(constants::db::COLLECTION_DISCUSSES);
    m_article_id = max(m_solution_id, max(m_discussion_id, m_announcement_id));

    // TODO: 输出 ID 信息
    cout << "Max Problem ID: " << m_problem_id << endl;
    cout << "Max Announcement ID: " << m_announcement_id << endl;
}

MoDB::~MoDB() {
    // 析构函数实现
}