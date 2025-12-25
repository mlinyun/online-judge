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
#include "constants/judge.h"
#include "constants/user.h"
#include "utils/id_generator.hpp"  // 唯一 ID 生成器
#include "utils/response.h"        // 统一响应工具

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
using constants::db::COLLECTION_STATUS_RECORDS;
using constants::db::COLLECTION_USERS;
using constants::db::DATABASE_NAME;

using namespace std;

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

// 局部静态特性的方式实现单实例模式
MoDB *MoDB::GetInstance() {
    static MoDB modb;
    return &modb;
}

// ------------------------------ 用户模块 Start ------------------------------
/**
 * 功能：用户注册
 * 权限：所有用户均可以注册
 * @name UserRegister
 * @brief 处理新用户注册请求
 * @param registerjson Json(NickName, Account, PassWord, PersonalProfile, School, Major)
 * @return Json(success, code, message, data(UserId))
 */
Json::Value MoDB::UserRegister(Json::Value &registerjson) {
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

        // 判断账户是否存在
        mongocxx::cursor cursor = usercoll.find({make_document(kvp("Account", account.data()))});
        if (cursor.begin() != cursor.end()) {
            return response::UserAccountExists();
        }
        // 检查昵称是否存在
        cursor = usercoll.find({make_document(kvp("NickName", nickname.data()))});
        if (cursor.begin() != cursor.end()) {
            return response::UserNicknameExists();
        }

        // 生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        // 获取当前时间字符串
        string jointime = GetTime();
        // 默认头像
        string avatar = "http://127.0.0.1:8081/api/image/1";
        // 默认用户权限为普通用户（3）
        int authority = constants::user::USER_AUTHORITY_ORDINARY;
        // 设置默认的个人简介
        if (personalprofile.empty()) {
            personalprofile = "这个人很懒，什么都没有留下。";
        }
        // 构造用户文档
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Avatar" << avatar.data() << "NickName" << nickname.data() << "Account"
                 << account.data() << "PassWord" << password.data() << "PersonalProfile" << personalprofile.data()
                 << "School" << school.data() << "Major" << major.data() << "JoinTime" << jointime.data()
                 << "CommentLikes" << open_array << close_array << "Solves" << open_array << close_array << "SubmitNum"
                 << 0 << "ACNum" << 0 << "Authority" << authority;
        // 插入用户文档
        auto result = usercoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            return response::DatabaseError("数据库插入失败！");
        }

        // 构建返回数据
        Json::Value data;
        data["UserId"] = std::to_string(id);
        return response::Success("注册成功！", data);
    } catch (const std::exception &e) {
        return response::InternalError("服务器内部错误：" + std::string(e.what()));
    }
}

/**
 * 功能：用户登录
 * 权限：所有用户均可以登录（被封禁的除外：未实现）
 * @name UserLogin
 * @brief 处理用户登录请求
 * @param loginjson Json(Account, PassWord)
 * @return Json(success, code, message, data(_id, NickName, Avatar, CommentLikes, Solves, Authority))
 */
Json::Value MoDB::UserLogin(Json::Value &loginjson) {
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
            return response::UserLoginFailed();
        }
        // 匹配成功
        // 解析用户信息并返回
        Json::Value user_info;
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), user_info);
        }
        return response::Success("登录成功！", user_info);
    } catch (const std::exception &e) {
        return response::InternalError("服务器内部错误：" + std::string(e.what()));
    }
}

/**
 * 功能：查询用户信息
 * 权限：只允许用户本人查询
 * @name SelectUserInfo
 * @brief 查询指定用户的信息
 * @param queryjson Json(UserId)
 * @return Json(success, code, message, data(_id, Avatar, NickName, PersonalProfile, School, Major, JoinTime, Solves,
 * ACNum, SubmitNum))
 */
Json::Value MoDB::SelectUserInfo(Json::Value &queryjson) {
    try {
        // 提取用户 ID
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
            return response::UserNotFound("未查询到该用户信息！");
        }

        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }

        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：查询用户信息（在设置页面修改用户时使用）
 * 权限：只允许用户本人查询
 * @name SelectUserUpdateInfo
 * @brief 查询指定用户的信息（用于用户设置页面的显示和修改）
 * @param queryjson Json(UserId)
 * @return Json(success, code, message, data(_id, Avatar, NickName, PersonalProfile, School, Major))
 */
Json::Value MoDB::SelectUserUpdateInfo(Json::Value &queryjson) {
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
            return response::UserNotFound("未查询到该用户信息！");
        }

        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：更新用户信息
 * 权限：只允许用户本人或者管理员修改
 * @name UpdateUserInfo
 * @brief 更新指定用户的信息
 * @param updatejson Json(UserId, Avatar, NickName, PersonalProfile, School, Major)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateUserInfo(Json::Value &updatejson) {
    try {
        // 获取用户 ID 和更新信息
        int64_t userid = stoll(updatejson["UserId"].asString());
        string avatar = updatejson["Avatar"].asString();
        string nickname = updatejson["NickName"].asString();
        string personalprofile = updatejson["PersonalProfile"].asString();
        string school = updatejson["School"].asString();
        string major = updatejson["Major"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];
        // 检查昵称是否存在且不为当前用户
        bsoncxx::builder::basic::document filter{};
        filter.append(kvp("NickName", nickname));
        filter.append(kvp("_id", make_document(kvp("$ne", userid))));
        mongocxx::cursor cursor = usercoll.find(filter.view());
        if (cursor.begin() != cursor.end()) {
            return response::UserNicknameExists();
        }
        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Avatar" << avatar.data() << "NickName" << nickname.data()
                 << "PersonalProfile" << personalprofile.data() << "School" << school.data() << "Major" << major.data()
                 << close_document;
        // 执行更新操作
        auto result = usercoll.update_one({make_document(kvp("_id", userid))}, document.view());
        if (!result || result->matched_count() == 0) {
            return response::UserNotFound("未找到对应用户，更新失败！");
        }
        // 构建返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("更新成功！", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：删除用户
 * 权限：只允许管理员删除
 * @name DeleteUser
 * @brief 删除指定用户
 * @param deletejson Json(UserId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteUser(Json::Value &deletejson) {
    try {
        // 提取用户 ID
        int64_t userid = stoll(deletejson["UserId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        // 执行删除操作
        auto result = usercoll.delete_one({make_document(kvp("_id", userid))});
        // 检查删除结果
        if (!result || result->deleted_count() < 1) {
            return response::UserNotFound("用户不存在，无法删除！");
        }
        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->deleted_count());
        return response::Success("删除成功！", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：用户排名查询
 * 权限：所有用户均可查询
 * @name SelectUserRank
 * @brief 分页查询用户排名列表
 * @param queryjson Json(Page, PageSize)
 * @return Json(success, code, message, data(List[{_id, Rank, Avatar, NickName, PersonalProfile, SubmitNum, ACNum}],
 * Total))
 */
Json::Value MoDB::SelectUserRank(Json::Value &queryjson) {
    try {
        // 获取分页查询参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        // 获取总用户数
        auto total = static_cast<int>(usercoll.count_documents({}));

        // 构造查询管道
        bsoncxx::builder::stream::document document{};
        // ACNum 值越大，越靠前；若 ACNum 相同，则 SubmitNum 越小排名越高，如果 ACNum 和 SubmitNum 都相同，则按 _id 排序
        document << "ACNum" << -1 << "SubmitNum" << 1 << "_id" << 1;

        // 构造聚合管道
        mongocxx::pipeline pipe;
        // 排序、跳过和限制
        pipe.sort(document.view());
        pipe.skip(skip);
        pipe.limit(pagesize);
        document.clear();

        // 只返回需要的字段
        document << "Avatar" << 1 << "NickName" << 1 << "PersonalProfile" << 1 << "SubmitNum" << 1 << "ACNum" << 1;
        pipe.project(document.view());

        Json::Reader reader;
        Json::Value list(Json::arrayValue);
        // 执行聚合查询
        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        // 添加 Rank 排名
        int currank = (page - 1) * pagesize + 1;
        for (int i = 0; i < list.size(); i++) {
            list[i]["Rank"] = currank++;
        }
        return response::SuccessList("查询成功", list, total);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：通过 UserId 获取用户排名值
 * 权限：只允许用户本人查询
 * @name SelectUserRankValue
 * @brief 通过 UserId 获取用户的排名值
 * @param queryjson Json(UserId)
 * @return Json(success, code, message, data(Rank))
 */
Json::Value MoDB::SelectUserRankValue(Json::Value &queryjson) {
    try {
        int64_t userid = stoll(queryjson["UserId"].asString());

        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        auto user_opt = usercoll.find_one(make_document(kvp("_id", userid)));
        if (!user_opt) {
            return response::UserNotFound();
        }

        auto view = user_opt->view();
        auto get_int = [](const bsoncxx::document::view &v, const char *key) -> int64_t {
            auto ele = v[key];
            if (!ele) return 0;
            if (ele.type() == bsoncxx::type::k_int32) return ele.get_int32().value;
            if (ele.type() == bsoncxx::type::k_int64) return ele.get_int64().value;
            return 0;
        };

        int64_t acnum = get_int(view, "ACNum");
        int64_t submitnum = get_int(view, "SubmitNum");

        // 统计“比该用户更靠前”的用户数量：
        // ACNum 更大；或 ACNum 相同但 SubmitNum 更小；或两者相同但 _id 更小（与排行榜的 _id 升序一致）
        bsoncxx::builder::stream::document filter;
        filter << "$or" << open_array
               << open_document << "ACNum" << open_document << "$gt" << acnum << close_document << close_document
               << open_document << "ACNum" << acnum << "SubmitNum" << open_document << "$lt" << submitnum << close_document
               << close_document
               << open_document << "ACNum" << acnum << "SubmitNum" << submitnum << "_id" << open_document << "$lt" << userid
               << close_document << close_document
               << close_array;

        int rank = static_cast<int>(usercoll.count_documents(filter.view())) + 1;
        Json::Value data;
        data["Rank"] = rank;
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：分页查询用户列表
 * 权限：只允许管理员查询
 * @name SelectUserSetInfo
 * @brief 分页查询用户列表（管理员权限）
 * @param queryjson Json(Page, PageSize, SearchInfo(NickName, School, Major, Account))
 * @return Json(success, code, message, data(List[{_id, NickName, PersonalProfile, School, Major, JoinTime}], Total))
 */
Json::Value MoDB::SelectUserSetInfo(Json::Value &queryjson) {
    try {
        // 提取查询信息
        Json::Value searchinfo = queryjson["SearchInfo"];
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];
        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 查询用户名（NickName）
        if (searchinfo["NickName"].asString().size() > 0) {
            document << "NickName" << open_document << "$regex" << searchinfo["NickName"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询学校（School）
        if (searchinfo["School"].asString().size() > 0) {
            document << "School" << open_document << "$regex" << searchinfo["School"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询专业（Major）
        if (searchinfo["Major"].asString().size() > 0) {
            document << "Major" << open_document << "$regex" << searchinfo["Major"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询账号（Account）
        if (searchinfo["Account"].asString().size() > 0) {
            document << "Account" << open_document << "$regex" << searchinfo["Account"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = usercoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 投影
        document << "NickName" << 1 << "PersonalProfile" << 1 << "School" << 1 << "Major" << 1 << "JoinTime" << 1;
        pipe.project(document.view());

        Json::Value list(Json::arrayValue);
        cursor = usercoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList("查询成功", list, total);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：更新用户题目信息
 * @name UpdateUserProblemInfo
 * @brief 更新用户的题目完成情况
 * @param updatejson Json(UserId, ProblemId, Status)
 * @return bool(如果题目第一次 AC 则返回 true，否则返回 false)
 */
bool MoDB::UpdateUserProblemInfo(Json::Value &updatejson) {
    try {
        // 提取用户 ID、题目 ID 和状态
        int64_t userid = stoll(updatejson["UserId"].asString());
        int64_t problemid = stoll(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        // 将用户提交数目加一
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

/**
 * 功能：通过 UserId 获取用户名 NickName
 * @name GetNickNameByUserId
 * @brief 通过 UserId 获取用户名 NickName
 * @param userid string(UserId)
 * @return string(NickName)
 */
std::string MoDB::GetNickNameByUserId(const std::string &userid) {
    try {
        // 提取用户 ID
        int64_t id = stoll(userid);

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        // 构造查询管道
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", id))});

        // 只查询 NickName 字段
        document << "NickName" << 1;
        pipe.project(document.view());

        // 执行查询
        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        // 检查是否有结果
        if (cursor.begin() == cursor.end()) {
            return "";
        }

        // 解析结果
        Json::Reader reader;
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return data["NickName"].asString();
    } catch (const std::exception &e) {
        return "";
    }
}

/**
 * 功能：用户修改密码
 * 权限：只允许用户本人修改
 * @name UpdateUserPassword
 * @brief 更新指定用户的密码
 * @param updatejson Json(UserId, OldPassWord, NewPassWord)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateUserPassword(Json::Value &updatejson) {
    try {
        // 获取用户 ID 和密码信息
        int64_t userid = stoll(updatejson["UserId"].asString());
        string oldpassword = updatejson["OldPassWord"].asString();
        string newpassword = updatejson["NewPassWord"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        // 检查旧密码是否正确
        bsoncxx::builder::basic::document filter{};
        filter.append(kvp("_id", userid));
        filter.append(kvp("PassWord", oldpassword));
        mongocxx::cursor cursor = usercoll.find(filter.view());
        if (cursor.begin() == cursor.end()) {
            return response::UserOldPasswordWrong();
        }

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "PassWord" << newpassword.data() << close_document;
        // 执行更新操作，确保旧密码匹配（避免检查通过后密码被并发修改导致越权更新）
        auto result = usercoll.update_one(filter.view(), document.view());
        if (!result) {
            return response::DatabaseError("密码修改失败！");
        }
        if (result->matched_count() == 0) {
            // 理论上前面已校验旧密码，这里兜底处理并发修改等情况
            return response::UserOldPasswordWrong();
        }
        // 构建返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("密码修改成功！", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
/**
 * 功能：查询题目信息（单条）
 * 权限：所有用户均可查询
 * @name SelectProblemInfo
 * @brief 查询指定题目的信息
 * @param queryjson Json(ProblemId)
 * @return Json(success, code, message, data(_id, Title, Description, TimeLimit, MemoryLimit, JudgeNum, SubmitNum,
 * ACNum, UserNickName, Tags[]))
 */
Json::Value MoDB::SelectProblemInfo(Json::Value &queryjson) {
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

        // 执行查询
        mongocxx::cursor cursor = problemcoll.aggregate(pipe);
        // 检查是否有结果
        if (cursor.begin() == cursor.end()) {
            return response::ProblemNotFound();
        }
        // 解析结果
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询题目信息（单条）
 * 权限：只允许管理员查询
 * @name SelectProblemInfoByAdmin
 * @brief 查询指定题目的信息（管理员权限）
 * @param queryjson Json(ProblemId)
 * @return Json(success, code, message, data(_id, Title, Description, TimeLimit, MemoryLimit, UserNickName, JudgeNum,
 * Tags[]))
 */
Json::Value MoDB::SelectProblemInfoByAdmin(Json::Value &queryjson) {
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
            return response::ProblemNotFound();
        }

        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：插入题目（管理员权限）
 * 权限：只允许管理员插入
 * @name InsertProblem
 * @brief 插入新题目
 * @param insertjson Json(Title, Description, TimeLimit, MemoryLimit, JudgeNum, UserNickName, Tags[])
 * @return Json(success, code, message, data(ProblemId))
 */
Json::Value MoDB::InsertProblem(Json::Value &insertjson) {
    try {
        // 构造题目 ID，使用雪花算法生成唯一 ID
        int64_t problemid = IDGenerator::Instance().NextId();
        string title = insertjson["Title"].asString();
        string description = insertjson["Description"].asString();
        int timelimit = insertjson["TimeLimit"].isString() ? stoi(insertjson["TimeLimit"].asString())
                                                           : insertjson["TimeLimit"].asInt();
        int memorylimit = insertjson["MemoryLimit"].isString() ? stoi(insertjson["MemoryLimit"].asString())
                                                               : insertjson["MemoryLimit"].asInt();
        int judgenum = insertjson["JudgeNum"].isString() ? stoi(insertjson["JudgeNum"].asString())
                                                         : insertjson["JudgeNum"].asInt();
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
        // 插入标签数组
        for (int i = 0; i < insertjson["Tags"].size(); i++) {
            string tag = insertjson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << finalize;
        auto result = problemcoll.insert_one(doc.view());

        if ((*result).result().inserted_count() < 1) {
            return response::DatabaseError("数据库插入失败！");
        }
        Json::Value data;
        data["ProblemId"] = (Json::Int64)problemid;
        return response::Success("创建成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新题目信息（管理员权限）
 * @name UpdateProblem
 * @brief 更新指定题目的信息
 * @param updatejson Json(ProblemId, Title, Description, TimeLimit, MemoryLimit, JudgeNum, UserNickName, Tags[])
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateProblem(Json::Value &updatejson) {
    try {
        // 提取题目信息（兼容字符串和数字类型）
        int problemid = updatejson["ProblemId"].isString() ? stoi(updatejson["ProblemId"].asString())
                                                           : updatejson["ProblemId"].asInt();
        string title = updatejson["Title"].asString();
        string description = updatejson["Description"].asString();
        int timelimit = updatejson["TimeLimit"].isString() ? stoi(updatejson["TimeLimit"].asString())
                                                           : updatejson["TimeLimit"].asInt();
        int memorylimit = updatejson["MemoryLimit"].isString() ? stoi(updatejson["MemoryLimit"].asString())
                                                               : updatejson["MemoryLimit"].asInt();
        int judgenum = updatejson["JudgeNum"].isString() ? stoi(updatejson["JudgeNum"].asString())
                                                         : updatejson["JudgeNum"].asInt();
        string usernickname = updatejson["UserNickName"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        bsoncxx::builder::stream::document document{};
        auto in_array = document << "$set" << open_document << "Title" << title.data() << "Description"
                                 << description.data() << "TimeLimit" << timelimit << "MemoryLimit" << memorylimit
                                 << "JudgeNum" << judgenum << "UserNickName" << usernickname.data() << "Tags"
                                 << open_array;
        // 插入标签数组
        for (int i = 0; i < updatejson["Tags"].size(); i++) {
            string tag = updatejson["Tags"][i].asString();
            in_array = in_array << tag.data();
        }
        bsoncxx::document::value doc = in_array << close_array << close_document << finalize;

        // 执行更新操作
        auto result = problemcoll.update_one({make_document(kvp("_id", problemid))}, doc.view());
        if (!result || result->matched_count() == 0) {
            return response::ProblemNotFound();
        }
        // 构建返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("更新成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除题目
 * 权限：只允许管理员删除
 * @name DeleteProblem
 * @brief 删除指定题目
 * @param deletejson Json(ProblemId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteProblem(Json::Value &deletejson) {
    try {
        // 提取题目 ID
        int64_t problemid = stoll(deletejson["ProblemId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        // 执行删除操作
        auto result = problemcoll.delete_one({make_document(kvp("_id", problemid))});

        // 检查删除结果
        if ((*result).deleted_count() < 1) {
            return response::ProblemNotFound();
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>((*result).deleted_count());
        return response::Success("删除成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页获取题目列表
 * 权限：所有用户均可查询
 * @name SelectProblemList
 * @brief 分页查询题目列表
 * @param queryjson Json(Page, PageSize, SearchInfo(Id, Title, Tags[]))
 * @return Json(success, code, message, data(List[{_id, Title, SubmitNum, CENum, ACNum, WANum, RENum,
 * TLENum, MLENum, SENum, Tags}], Total))
 */
Json::Value MoDB::SelectProblemList(Json::Value &queryjson) {
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
            int64_t id = stoll(searchinfo["Id"].asString());
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
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行
        document << "Title" << 1 << "SubmitNum" << 1 << "CENum" << 1 << "ACNum" << 1 << "WANum" << 1 << "RENum" << 1
                 << "TLENum" << 1 << "MLENum" << 1 << "SENum" << 1 << "Tags" << 1;
        pipe.project(document.view());

        Json::Value list(Json::arrayValue);
        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页获取题目列表
 * 权限：只允许管理员查询
 * @name SelectProblemListByAdmin
 * @brief 分页查询题目列表（管理员权限）
 * @param queryjson Json(Page, PageSize)
 * @return Json(success, code, message, data(List[{_id, Title, SubmitNum, CENum, ACNum, WANum, RENum,
 * TLENum, MLENum, SENum, Tags}], Total))
 */
Json::Value MoDB::SelectProblemListByAdmin(Json::Value &queryjson) {
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
            int64_t id = stoll(searchinfo["Id"].asString());
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

        // 查询创建者昵称
        if (searchinfo["UserNickName"].asString().size() > 0) {
            document << "UserNickName" << open_document << "$regex" << searchinfo["UserNickName"].asString()
                     << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = problemcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        // 排序
        pipe.sort({make_document(kvp("_id", 1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);
        // 进行（<< "ProblemId" << "$_id"）
        document << "Title" << 1 << "SubmitNum" << 1 << "CENum" << 1 << "ACNum" << 1 << "WANum" << 1 << "RENum" << 1
                 << "TLENum" << 1 << "MLENum" << 1 << "SENum" << 1 << "Tags" << 1;
        pipe.project(document.view());

        Json::Value list(Json::arrayValue);
        cursor = problemcoll.aggregate(pipe);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新题目的状态数量
 * @name UpdateProblemStatusNum
 * @brief 根据提交状态更新题目的状态数量
 * @param updatejson Json(ProblemId, Status)
 * @return bool
 */
bool MoDB::UpdateProblemStatusNum(Json::Value &updatejson) {
    try {
        // 提取题目 ID 和状态
        int64_t problemid = stoll(updatejson["ProblemId"].asString());
        int status = stoi(updatejson["Status"].asString());

        // 根据判题状态选择更新对应的字段
        const char *status_field = nullptr;
        if (status == constants::judge::STATUS_COMPILE_ERROR) {
            status_field = constants::judge::FIELD_COMPILE_ERROR;
        } else if (status == constants::judge::STATUS_ACCEPTED) {
            status_field = constants::judge::FIELD_ACCEPTED;
        } else if (status == constants::judge::STATUS_WRONG_ANSWER) {
            status_field = constants::judge::FIELD_WRONG_ANSWER;
        } else if (status == constants::judge::STATUS_RUNTIME_ERROR) {
            status_field = constants::judge::FIELD_RUNTIME_ERROR;
        } else if (status == constants::judge::STATUS_TIME_LIMIT_EXCEEDED) {
            status_field = constants::judge::FIELD_TIME_LIMIT_EXCEEDED;
        } else if (status == constants::judge::STATUS_MEMORY_LIMIT_EXCEEDED) {
            status_field = constants::judge::FIELD_MEMORY_LIMIT_EXCEEDED;
        } else if (status == constants::judge::STATUS_SYSTEM_ERROR) {
            status_field = constants::judge::FIELD_SYSTEM_ERROR;
        }

        // 如果状态字段无效，则返回 false
        if (status_field == nullptr) {
            return false;
        }

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection problemcoll = (*client)[DATABASE_NAME][COLLECTION_PROBLEMS];

        const std::string status_field_name(status_field);
        // 构造更新文档，增加提交数和对应状态数
        auto update_doc = make_document(kvp("$inc", make_document(kvp("SubmitNum", 1), kvp(status_field_name, 1))));
        // 执行更新操作
        auto result = problemcoll.update_one({make_document(kvp("_id", problemid))}, update_doc.view());
        // 返回更新结果
        return static_cast<bool>(result && result->modified_count() > 0);
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 题目模块 End ------------------------------

// ------------------------------ 标签模块 Start ------------------------------
/**
 * 功能：获取题目的所有标签
 * 权限：所有用户均可查询
 * @name GetProblemTags
 * @brief 获取题目的所有标签
 * @param void
 * @return Json(success, code, message, data(Tags[]))
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
 * 功能：添加公告
 * 权限：只允许管理员添加
 * @name InsertAnnouncement
 * @brief 插入新公告
 * @param insertjson Json(Title, Content, UserId, Level, Active)
 * @return Json(success, code, message, data(AnnouncementId))
 */
Json::Value MoDB::InsertAnnouncement(Json::Value &insertjson) {
    try {
        // 构造公告 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        int level = stoi(insertjson["Level"].asString());
        bool isactive = insertjson["Active"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 构造插入文档
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "UserId" << userid
                 << "Views" << 0 << "Comments" << 0 << "Level" << level << "Active" << isactive << "CreateTime"
                 << GetTime().data() << "UpdateTime" << GetTime().data();

        // 执行插入操作
        auto result = announcementcoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            return response::DatabaseError("数据库插入失败！");
        }

        // 构造返回数据
        Json::Value data;
        data["AnnouncementId"] = std::to_string(id);
        return response::Success("添加公告成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询公告详细信息，并将其浏览量加 1
 * 权限：所有用户均可查询
 * @name SelectAnnouncement
 * @brief 查询公告详细信息，并将其浏览量加 1
 * @param queryjson Json(AnnouncementId)
 * @return Json(success, code, message, data(_id, Title, Content, Views, Comments, CreateTime, UpdateTime))
 */
Json::Value MoDB::SelectAnnouncement(Json::Value &queryjson) {
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
            return response::Fail(error_code::ANNOUNCEMENT_NOT_FOUND, "公告不存在！");
        }

        Json::Reader reader;
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询公告的详细信息，主要是编辑时的查询
 * 权限：只允许管理员查询
 * @name SelectAnnouncementByEdit
 * @brief 查询公告的详细信息，主要是编辑时的查询（管理员权限）
 * @param queryjson Json(AnnouncementId)
 * @return Json(success, code, message, data(_id, Title, Content, Level, Active))
 */
Json::Value MoDB::SelectAnnouncementByEdit(Json::Value &queryjson) {
    try {
        // 提取公告 ID
        int64_t announcementid = stoll(queryjson["AnnouncementId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", announcementid))});
        document << "Title" << 1 << "Content" << 1 << "Level" << 1 << "Active" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = announcementcoll.aggregate(pipe);

        Json::Reader reader;
        if (cursor.begin() == cursor.end()) {
            return response::Fail(error_code::ANNOUNCEMENT_NOT_FOUND, "公告不存在！");
        }
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新公告
 * 权限：只允许管理员修改
 * @name UpdateAnnouncement
 * @brief 更新指定公告的信息
 * @param updatejson Json(AnnouncementId, Title, Content, Level, Active)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateAnnouncement(Json::Value &updatejson) {
    try {
        // 提取公告信息
        int64_t announcementid = stoll(updatejson["AnnouncementId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();
        int level = stoi(updatejson["Level"].asString());
        bool isactive = updatejson["Active"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "Level"
                 << level << "Active" << isactive << "UpdateTime" << GetTime().data() << close_document;

        // 执行更新
        auto result = announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());
        if (!result || result->matched_count() == 0) {
            return response::Fail(error_code::ANNOUNCEMENT_NOT_FOUND, "公告不存在！");
        }
        // 构建返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("更新公告成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除公告
 * 权限：只允许管理员删除
 * @name DeleteAnnouncement
 * @brief 删除指定公告
 * @param deletejson Json(AnnouncementId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteAnnouncement(Json::Value &deletejson) {
    try {
        // 提取公告 ID
        int64_t announcementid = stoll(deletejson["AnnouncementId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 执行删除操作
        auto result = announcementcoll.delete_one({make_document(kvp("_id", announcementid))});
        if ((*result).deleted_count() < 1) {
            return response::Fail(error_code::ANNOUNCEMENT_NOT_FOUND, "公告不存在或已删除！");
        }
        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>((*result).deleted_count());
        return response::Success("删除公告成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询公告列表（激活的公告）
 * 权限：所有用户均可查询
 * @name SelectAnnouncementList
 * @brief 分页查询公告列表
 * @param queryjson Json(Page, PageSize)
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, CreateTime}], Total))
 */
Json::Value MoDB::SelectAnnouncementList(Json::Value &queryjson) {
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

        // 只查询激活的公告
        pipe.match({make_document(kvp("Active", true))});
        pipetot.match({make_document(kvp("Active", true))});

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = announcementcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.sort({make_document(kvp("Level", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);

        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "Level" << 1 << "CreateTime" << 1;
        pipe.project(document.view());

        cursor = announcementcoll.aggregate(pipe);

        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询所有公告列表（包括未激活的公告）
 * 权限：只允许管理员查询
 * @name SelectAnnouncementListByAdmin
 * @brief 分页查询所有公告列表
 * @param queryjson Json(Page, PageSize)
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, Active, CreateTime}], Total))
 */
Json::Value MoDB::SelectAnnouncementListByAdmin(Json::Value &queryjson) {
    try {
        // 提取查询信息
        Json::Value searchinfo = queryjson["SearchInfo"];
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];
        Json::Reader reader;
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 查询标题（Title）
        if (searchinfo["Title"].asString().size() > 0) {
            document << "Title" << open_document << "$regex" << searchinfo["Title"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询公告级别（Level）
        if (searchinfo["Level"].asString().size() > 0) {
            int level = stoi(searchinfo["Level"].asString());
            pipe.match({make_document(kvp("Level", level))});
            pipetot.match({make_document(kvp("Level", level))});
        }

        // 查询激活状态（Active）
        if (searchinfo["Active"].asString().size() > 0) {
            bool isactive = searchinfo["Active"].asBool();
            pipe.match({make_document(kvp("Active", isactive))});
            pipetot.match({make_document(kvp("Active", isactive))});
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = announcementcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.sort({make_document(kvp("Level", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);

        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "Level" << 1 << "Active" << 1 << "CreateTime"
                 << 1;
        pipe.project(document.view());

        cursor = announcementcoll.aggregate(pipe);

        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：设置公告激活状态
 * 权限：只允许管理员操作
 * @name UpdateAnnouncementActive
 * @brief 设置公告是否激活
 * @param updatejson Json(AnnouncementId, Active)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateAnnouncementActive(Json::Value &updatejson) {
    try {
        // 提取公告 ID 和激活状态
        int64_t announcementid = stoll(updatejson["AnnouncementId"].asString());
        bool isactive = updatejson["Active"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Active" << isactive << close_document;

        // 执行更新操作
        auto result = announcementcoll.update_one({make_document(kvp("_id", announcementid))}, document.view());
        if (!result || result->modified_count() < 1) {
            return response::Fail(error_code::ANNOUNCEMENT_NOT_FOUND, "公告不存在！");
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("设置公告状态成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新公告的评论数量
 * @name UpdateAnnouncementComments
 * @brief 更新公告的评论数量
 * @param updatejson Json(ArticleId, Num)
 * @return bool
 */
bool MoDB::UpdateAnnouncementComments(Json::Value &updatejson) {
    try {
        // 提取公告 ID 和评论数
        int64_t articleid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection announcementcoll = (*client)[DATABASE_NAME][COLLECTION_ANNOUNCEMENTS];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;

        // 执行更新操作
        auto result = announcementcoll.update_one({make_document(kvp("_id", articleid))}, document.view());
        // 检查更新结果
        if (!result || result->modified_count() < 1) {
            return false;
        }

        // 更新成功
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
/**
 * 功能：添加讨论
 * 权限：只允许普通用户及以上添加
 * @name InsertDiscuss
 * @brief 插入新讨论
 * @param insertjson Json(Title, Content, ParentId, UserId)
 * @return Json(success, code, message, data(DiscussId))
 */
Json::Value MoDB::InsertDiscuss(Json::Value &insertjson) {
    try {
        // 构造讨论 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t parentid = atoll(insertjson["ParentId"].asString().data());
        int64_t userid = atoll(insertjson["UserId"].asString().data());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 构造插入文档
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "ParentId" << parentid
                 << "UserId" << userid << "Views" << 0 << "Comments" << 0 << "CreateTime" << GetTime().data()
                 << "UpdateTime" << GetTime().data();

        // 执行插入操作
        auto result = discusscoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            return response::DatabaseError("数据库插入失败！");
        }
        // 构造返回数据
        Json::Value data;
        data["DiscussId"] = std::to_string(id);
        return response::Success("添加讨论成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询讨论的详细内容，并且将其浏览量加 1
 * 权限：所有用户均可查询
 * @name SelectDiscuss
 * @brief 查询讨论的详细内容，并且将其浏览量加 1
 * @param queryjson Json(DiscussId)
 * @return Json(success, code, message, data(_id, Title, Content, Views, Comments, CreateTime, UpdateTime, User{_id,
 * Avatar, NickName}))
 */
Json::Value MoDB::SelectDiscuss(Json::Value &queryjson) {
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

        // 查询 Content
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document.clear();
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Content" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "UpdateTime" << 1 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        if (cursor.begin() == cursor.end()) {
            return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
        }

        Json::Reader reader;
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询讨论的详细信息，主要是编辑时的查询
 * 权限：只允许讨论作者本人或者管理员查询
 * @name SelectDiscussByEdit
 * @brief 查询讨论的详细信息，主要是编辑时的查询（讨论作者本人或管理员权限）
 * @param queryjson Json(DiscussId)
 * @return Json(success, code, message, data(_id, Title, Content, UserId))
 */
Json::Value MoDB::SelectDiscussByEdit(Json::Value &queryjson) {
    try {
        // 提取讨论 ID
        int64_t discussid = stoll(queryjson["DiscussId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 构造查询文档
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", discussid))});
        document << "Title" << 1 << "UserId" << 1 << "Content" << 1;
        pipe.project(document.view());
        // 执行查询
        mongocxx::cursor cursor = discusscoll.aggregate(pipe);

        Json::Reader reader;

        // 检查讨论是否存在
        if (cursor.begin() == cursor.end()) {
            return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
        }

        // 构造返回数据
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新讨论
 * 权限：只允许讨论作者本人或者管理员修改
 * @name UpdateDiscuss
 * @brief 更新指定讨论的信息
 * @param updatejson Json(DiscussId, Title, Content)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateDiscuss(Json::Value &updatejson) {
    try {
        // 提取讨论信息
        int64_t discussid = stoll(updatejson["DiscussId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "UpdateTime"
                 << GetTime().data() << close_document;

        // 执行更新
        auto result = discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());
        if (!result || result->matched_count() == 0) {
            return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在！");
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = (result->modified_count() > 0);
        return response::Success("更新讨论成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除讨论
 * 权限：只允许讨论作者本人或者管理员删除
 * @name DeleteDiscuss
 * @brief 删除指定讨论
 * @param deletejson Json(DiscussId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteDiscuss(Json::Value &deletejson) {
    try {
        // 提取讨论 ID
        int64_t discussid = stoll(deletejson["DiscussId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 执行删除操作
        auto result = discusscoll.delete_one({make_document(kvp("_id", discussid))});
        if ((*result).deleted_count() < 1) {
            return response::Fail(error_code::DISCUSS_NOT_FOUND, "讨论不存在或已删除！");
        }
        // 构造返回数据
        Json::Value data;
        data["Result"] = (result->deleted_count() > 0);
        return response::Success("删除讨论成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询讨论
 * 权限：所有用户均可查询
 * @name SelectDiscussList
 * @brief 分页查询讨论列表
 * @param queryjson Json(Page, PageSize, SearchInfo(ParentId, UserId))
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, CreateTime, User{_id, Avatar,
 * NickName}}], Total))
 */
Json::Value MoDB::SelectDiscussList(Json::Value &queryjson) {
    try {
        // 提取分页参数（Page, PageSize 是必传参数）
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // SearchInfo 可选参数，需要安全处理
        int64_t parentid = 0;
        int64_t userid = 0;

        // 提取 SearchInfo 中的查询条件
        if (queryjson.isMember("SearchInfo") && queryjson["SearchInfo"].isObject()) {
            const Json::Value &searchinfo = queryjson["SearchInfo"];
            if (searchinfo.isMember("ParentId") && searchinfo["ParentId"].asString().size() > 0) {
                parentid = stoll(searchinfo["ParentId"].asString());
            }
            if (searchinfo.isMember("UserId") && searchinfo["UserId"].asString().size() > 0) {
                userid = stoll(searchinfo["UserId"].asString());
            }
        }

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        if (parentid > 0) {
            pipe.match({make_document(kvp("ParentId", parentid))});
            pipetot.match({make_document(kvp("ParentId", parentid))});
        }

        if (userid > 0) {
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        Json::Reader reader;
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }
        // 排序：根据创建时间降序
        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document.clear();
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "User._id" << 1
                 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        // 执行查询
        cursor = discusscoll.aggregate(pipe);
        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        std::cerr << "Error in SelectDiscussList: " << e.what() << std::endl;
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询讨论
 * 权限：只允许管理员查询
 * @name SelectDiscussListByAdmin
 * @brief 分页查询讨论列表（管理员权限）
 * @param queryjson Json(Page, PageSize, SearchInfo(Title, UserId, ParentId))
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, CreateTime, User{_id, Avatar,
 * NickName}}], Total))
 */
Json::Value MoDB::SelectDiscussListByAdmin(Json::Value &queryjson) {
    try {
        // 提取搜索参数
        Json::Value searchinfo = queryjson["SearchInfo"];
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 查询标题（Title）
        if (searchinfo["Title"].asString().size() > 0) {
            document << "Title" << open_document << "$regex" << searchinfo["Title"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询用户 ID（UserId）
        if (searchinfo["UserId"].asString().size() > 0) {
            int64_t userid = stoll(searchinfo["UserId"].asString());
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        // 查询父级 ID（ParentId）
        if (searchinfo["ParentId"].asString().size() > 0) {
            int64_t parentid = stoll(searchinfo["ParentId"].asString());
            pipe.match({make_document(kvp("ParentId", parentid))});
            pipetot.match({make_document(kvp("ParentId", parentid))});
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        Json::Reader reader;
        mongocxx::cursor cursor = discusscoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "ParentId" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = discusscoll.aggregate(pipe);
        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新讨论的评论数量
 * @name UpdateDiscussComments
 * @brief 更新讨论的评论数量
 * @param updatejson Json(ArticleId, Num)
 * @return bool
 */
bool MoDB::UpdateDiscussComments(Json::Value &updatejson) {
    try {
        // 提取讨论 ID 和评论数
        int64_t discussid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection discusscoll = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;

        // 执行更新
        auto result = discusscoll.update_one({make_document(kvp("_id", discussid))}, document.view());
        // 检查更新结果
        if (!result || result->modified_count() < 1) {
            return false;
        }

        // 更新成功
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

/**
 * 功能：获取讨论的作者 UserId
 * @name GetDiscussAuthorId
 * @brief 获取讨论的作者 UserId
 * @param discussId 讨论 ID
 * @return 作者 UserId，如果不存在返回空字符串
 */
std::string MoDB::GetDiscussAuthorId(int64_t discussId) {
    try {
        // 获取数据库连接
        auto client = pool.acquire();
        auto discussColl = (*client)[DATABASE_NAME][COLLECTION_DISCUSSES];

        // 构造查询条件
        bsoncxx::builder::stream::document filterBuilder{};
        filterBuilder << "_id" << discussId;

        // 构造 projection：只返回 UserId 字段
        bsoncxx::builder::stream::document projBuilder{};
        projBuilder << "UserId" << 1;

        mongocxx::options::find options;
        options.projection(projBuilder.view());

        // 执行查询
        auto result = discussColl.find_one(filterBuilder.view(), options);

        // 处理查询结果
        if (!result) {
            return "";
        }

        // 解析结果视图
        auto view = result->view();
        auto userid_element = view["UserId"];

        // 类型安全检查
        if (!userid_element || userid_element.type() != bsoncxx::type::k_int64) {
            return "";
        }

        return std::to_string(userid_element.get_int64().value);
    } catch (const std::exception &e) {
        // 如需日志可在此处打印，用于 debug
        std::cerr << "GetDiscussAuthorId error: " << e.what() << std::endl;
        return "";
    }
}

// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
/**
 * 功能：添加题解
 * 权限：只允许普通用户及以上添加
 * @name InsertSolution
 * @brief 插入新题解
 * @param insertjson Json(Title, Content, ParentId, UserId, Public)
 * @return Json(success, code, message, data(SolutionId))
 */
Json::Value MoDB::InsertSolution(Json::Value &insertjson) {
    try {
        // 构造题解 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        string title = insertjson["Title"].asString();
        string content = insertjson["Content"].asString();
        int64_t parentid = stoll(insertjson["ParentId"].asString());
        int64_t userid = stoll(insertjson["UserId"].asString());
        bool ispublic = insertjson["Public"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 构造插入文档
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "Title" << title.data() << "Content" << content.data() << "ParentId" << parentid
                 << "UserId" << userid << "Views" << 0 << "Comments" << 0 << "Public" << ispublic << "CreateTime"
                 << GetTime().data() << "UpdateTime" << GetTime().data();

        // 执行插入操作
        auto result = solutioncoll.insert_one(document.view());
        if ((*result).result().inserted_count() < 1) {
            return response::DatabaseError("数据库插入失败！");
        }

        // 构造返回数据
        Json::Value data;
        data["SolutionId"] = std::to_string(id);
        return response::Success("添加题解成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询题解的详细内容，并且将其浏览量加 1
 * 权限：所有用户均可查询
 * @name SelectSolution
 * @brief 查询题解的详细内容，并且将其浏览量加 1
 * @param queryjson Json(SolutionId)
 * @return Json(success, code, message, data(_id, Title, Content, Views, Comments, CreateTime, UpdateTime, User{_id,
 * Avatar, NickName}))
 */
Json::Value MoDB::SelectSolution(Json::Value &queryjson) {
    try {
        // 提取题解 ID
        int64_t solutionid = stoll(queryjson["SolutionId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 先检查题解是否存在且公开
        mongocxx::pipeline checkpipe;
        checkpipe.match({make_document(kvp("_id", solutionid), kvp("Public", true))});
        mongocxx::cursor checkcursor = solutioncoll.aggregate(checkpipe);
        if (checkcursor.begin() == checkcursor.end()) {
            return response::SolutionNotFound("题解不存在或未公开！");
        }

        // 浏览量加一
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Views" << 1 << close_document;
        solutioncoll.update_one({make_document(kvp("_id", solutionid))}, document.view());

        // 查询 Content
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", solutionid), kvp("Public", true))});
        document.clear();

        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Content" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1
                 << "UpdateTime" << 1 << "User._id" << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        // 执行聚合查询
        mongocxx::cursor cursor = solutioncoll.aggregate(pipe);
        if (cursor.begin() == cursor.end()) {
            return response::SolutionNotFound();
        }

        // 构造返回数据
        Json::Value data;
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：查询题解的详细信息，主要是编辑时的查询
 * 权限：只允许题解作者本人或者管理员查询
 * @name SelectSolutionByEdit
 * @brief 查询题解的详细信息，主要是编辑时的查询（题解
 * @param queryjson Json(SolutionId)
 * @return Json(success, code, message, data(_id, Title, Content, ParentId, UserId, Public))
 */
Json::Value MoDB::SelectSolutionByEdit(Json::Value &queryjson) {
    try {
        int64_t solutionid = stoll(queryjson["SolutionId"].asString());

        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;
        pipe.match({make_document(kvp("_id", solutionid))});
        document << "Title" << 1 << "Content" << 1 << "ParentId" << 1 << "UserId" << 1 << "Public" << 1;
        pipe.project(document.view());
        mongocxx::cursor cursor = solutioncoll.aggregate(pipe);

        Json::Reader reader;
        if (cursor.begin() == cursor.end()) {
            return response::SolutionNotFound();
        }
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新题解
 * 权限：只允许题解作者本人或者管理员修改
 * @name UpdateSolution
 * @brief 更新指定题解的信息
 * @param updatejson Json(SolutionId, Title, Content, Public)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::UpdateSolution(Json::Value &updatejson) {
    try {
        // 提取题解信息
        int64_t solutionid = stoll(updatejson["SolutionId"].asString());
        string title = updatejson["Title"].asString();
        string content = updatejson["Content"].asString();
        bool ispublic = updatejson["Public"].asBool();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        bsoncxx::builder::stream::document document{};
        document << "$set" << open_document << "Title" << title.data() << "Content" << content.data() << "Public"
                 << ispublic << "UpdateTime" << GetTime().data() << close_document;

        // 执行更新
        auto result = solutioncoll.update_one({make_document(kvp("_id", solutionid))}, document.view());
        if (!result || result->matched_count() == 0) {
            return response::SolutionNotFound();
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = (result->modified_count() > 0);
        return response::Success("更新题解成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除题解
 * 权限：只允许题解作者本人或者管理员删除
 * @name DeleteSolution
 * @brief 删除指定题解
 * @param deletejson Json(SolutionId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteSolution(Json::Value &deletejson) {
    try {
        // 提取题解 ID
        int64_t solutionid = stoll(deletejson["SolutionId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 执行删除操作
        auto result = solutioncoll.delete_one({make_document(kvp("_id", solutionid))});

        if ((*result).deleted_count() < 1) {
            return response::SolutionNotFound();
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = (result->deleted_count() > 0);
        return response::Success("删除题解成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询题解（公开题解）
 * 权限：所有用户均可查询
 * @name SelectSolutionList
 * @brief 分页查询题解列表
 * @param queryjson Json(Page, PageSize, SearchInfo(ParentId, UserId))
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, CreateTime, User{_id, Avatar,
 * NickName}}], Total))
 */
Json::Value MoDB::SelectSolutionList(Json::Value &queryjson) {
    try {
        // 提取分页参数（Page, PageSize 是必传参数）
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // SearchInfo 可选参数，需要安全处理
        int64_t parentid = 0;
        int64_t userid = 0;

        // 提取 SearchInfo 中的查询条件
        if (queryjson.isMember("SearchInfo") && queryjson["SearchInfo"].isObject()) {
            const Json::Value &searchinfo = queryjson["SearchInfo"];
            if (searchinfo.isMember("ParentId") && searchinfo["ParentId"].asString().size() > 0) {
                parentid = stoll(searchinfo["ParentId"].asString());
            }
            if (searchinfo.isMember("UserId") && searchinfo["UserId"].asString().size() > 0) {
                userid = stoll(searchinfo["UserId"].asString());
            }
        }

        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        if (parentid > 0) {
            // 匹配 ParentId
            pipetot.match({make_document(kvp("ParentId", parentid))});
            pipe.match({make_document(kvp("ParentId", parentid))});
        }
        if (userid > 0) {
            // 匹配 UserId
            pipetot.match({make_document(kvp("UserId", userid))});
            pipe.match({make_document(kvp("UserId", userid))});
        }

        // 只匹配公开的题解
        pipetot.match({make_document(kvp("Public", true))});
        pipe.match({make_document(kvp("Public", true))});

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = solutioncoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "User._id" << 1
                 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = solutioncoll.aggregate(pipe);
        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询题解
 * 权限：只允许管理员查询
 * @name SelectSolutionListByAdmin
 * @brief 分页查询题解列表（管理员权限）
 * @param queryjson Json(Page, PageSize, SearchInfo(Title, UserId, ParentId, Public))
 * @return Json(success, code, message, data(List[{_id, Title, Views, Comments, CreateTime, User{_id, Avatar,
 * NickName}}], Total))
 */
Json::Value MoDB::SelectSolutionListByAdmin(Json::Value &queryjson) {
    try {
        // 提取搜索参数
        Json::Value searchinfo = queryjson["SearchInfo"];
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];
        Json::Reader reader;
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        // 查询标题（Title）
        if (searchinfo["Title"].asString().size() > 0) {
            document << "Title" << open_document << "$regex" << searchinfo["Title"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 查询用户 ID（UserId）
        if (searchinfo["UserId"].asString().size() > 0) {
            int64_t userid = stoll(searchinfo["UserId"].asString());
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        // 查询父级 ID（ParentId）
        if (searchinfo["ParentId"].asString().size() > 0) {
            int64_t parentid = stoll(searchinfo["ParentId"].asString());
            pipe.match({make_document(kvp("ParentId", parentid))});
            pipetot.match({make_document(kvp("ParentId", parentid))});
        }

        // 查询公开状态（Public）
        if (searchinfo.isMember("Public")) {
            bool ispublic = searchinfo["Public"].asBool();
            pipe.match({make_document(kvp("Public", ispublic))});
            pipetot.match({make_document(kvp("Public", ispublic))});
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = solutioncoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        pipe.sort({make_document(kvp("CreateTime", -1))});
        pipe.skip(skip);
        pipe.limit(pagesize);
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());

        document.clear();
        document << "Title" << 1 << "Views" << 1 << "Comments" << 1 << "CreateTime" << 1 << "Public" << 1 << "User._id"
                 << 1 << "User.Avatar" << 1 << "User.NickName" << 1;
        pipe.project(document.view());

        cursor = solutioncoll.aggregate(pipe);
        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：更新题解的评论数
 * @name UpdateSolutionComments
 * @brief 更新题解的评论数
 * @param updatejson Json(ArticleId, Num)
 * @return bool
 */
bool MoDB::UpdateSolutionComments(Json::Value &updatejson) {
    try {
        // 提取题解 ID 和评论数
        int64_t articleid = stoll(updatejson["ArticleId"].asString());
        int num = stoi(updatejson["Num"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 构造更新文档
        bsoncxx::builder::stream::document document{};
        document << "$inc" << open_document << "Comments" << num << close_document;

        // 执行更新
        auto result = solutioncoll.update_one({make_document(kvp("_id", articleid))}, document.view());
        // 检查更新结果
        if (!result || result->modified_count() < 1) {
            return false;
        }

        // 更新成功
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

/**
 * 功能：获取题解的作者 UserId
 * @name GetSolutionAuthorId
 * @brief 获取题解的作者 UserId
 * @param solutionId 题解 ID
 * @return 作者 UserId，如果不存在返回空字符串
 */
std::string MoDB::GetSolutionAuthorId(int64_t solutionId) {
    try {
        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection solutioncoll = (*client)[DATABASE_NAME][COLLECTION_SOLUTIONS];

        // 构造查询条件
        bsoncxx::builder::stream::document filterBuilder{};
        filterBuilder << "_id" << solutionId;

        // 构造 projection：只返回 UserId 字段
        bsoncxx::builder::stream::document projBuilder{};
        projBuilder << "UserId" << 1;

        // 设置查询选项
        mongocxx::options::find options;
        options.projection(projBuilder.view());

        // 执行查询
        auto result = solutioncoll.find_one(filterBuilder.view(), options);

        // 处理查询结果
        if (!result) {
            return "";
        }
        // 解析结果视图
        auto view = result->view();
        auto userid_element = view["UserId"];

        // 类型安全检查
        if (!userid_element || userid_element.type() != bsoncxx::type::k_int64) {
            return "";
        }

        // 返回作者 UserId
        return std::to_string(userid_element.get_int64().value);
    } catch (const std::exception &e) {
        // 如需日志可在此处打印，用于 debug
        std::cerr << "GetSolutionAuthorId error: " << e.what() << std::endl;
        return "";
    }
}
// ------------------------------ 题解模块 End ------------------------------

// ------------------------------ 评论模块 Start ------------------------------
/**
 * 功能：根据文章类型更新对应的评论数
 * @name UpdateArticleComments
 * @brief 根据文章类型（Discuss/Solution/Announcement）更新对应的评论数
 * @param articleId 文章ID
 * @param articleType 文章类型
 * @param num 评论数变化量（正数增加，负数减少）
 * @return bool 更新是否成功
 */
bool MoDB::UpdateArticleComments(int64_t articleId, const std::string &articleType, int num) {
    Json::Value updatejson;
    updatejson["ArticleId"] = to_string(articleId);
    updatejson["Num"] = num;
    if (articleType == "Discuss") {
        return UpdateDiscussComments(updatejson);
    } else if (articleType == "Solution") {
        return UpdateSolutionComments(updatejson);
    } else if (articleType == "Announcement") {
        return UpdateAnnouncementComments(updatejson);
    }
    return false;
}

/**
 * 功能：插入父评论
 * 权限：普通用户及以上
 * @name InsertFatherComment
 * @brief 插入父评论
 * @param insertjson Json(ParentId, ParentType, Content, UserId)
 * @return Json(success, code, message, data(CommentId, CreateTime))
 */
Json::Value MoDB::InsertFatherComment(Json::Value &insertjson) {
    try {
        // 构造评论 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        int64_t parentid = stoll(insertjson["ParentId"].asString());
        string parenttype = insertjson["ParentType"].asString();
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        string createtime = GetTime();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        // 构造插入文档
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "ParentId" << parentid << "ParentType" << parenttype.data() << "Content"
                 << content.data() << "UserId" << userid << "Likes" << 0 << "CreateTime" << createtime.data()
                 << "Child_Comments" << open_array << close_array;

        // 执行插入操作
        auto result = commentcoll.insert_one(document.view());

        // 检查插入结果
        if (!result) {
            // 数据库操作本身失败（如连接中断等）
            return response::DatabaseError("数据库操作失败！");
        }
        if (result->result().inserted_count() < 1) {
            // 评论插入失败
            return response::CommentInsertFailed();
        }

        /**
         * 分布式事务一致性问题：确保评论插入和对应的评论数更新的一致性
         * 方案：补偿机制（最终一致性）
         * 采用"先插入评论，失败则补偿回滚"的策略
         * 步骤：
         * 1. 插入评论成功后，尝试更新对应的评论数
         * 2. 如果更新评论数失败，执行补偿操作，删除刚插入的评论
         * 3. 返回错误信息，提示用户稍后重试
         */

        // 如果父评论插入成功，更新对应的讨论或题解的评论数
        bool updateResult = UpdateArticleComments(parentid, parenttype, 1);
        // 如果更新评论数失败，执行补偿操作，删除刚插入的评论
        if (!updateResult) {
            commentcoll.delete_one({make_document(kvp("_id", id))});
            return response::DatabaseError("更新评论数失败，请稍后重试！");
        }

        // 构造返回数据
        Json::Value data;
        data["CommentId"] = to_string(id);
        data["CreateTime"] = createtime.data();
        return response::Success("评论成功", data);
    } catch (const std::exception &e) {
        cout << "InsertFatherComment error: " << e.what() << endl;
        return response::DatabaseError();
    }
}

/**
 * 功能：插入子评论
 * 权限：普通用户及以上
 * @name InsertSonComment
 * @brief 插入子评论
 * @param insertjson Json(ParentId, Content, UserId)
 * @return Json(success, code, message, data(CommentId, CreateTime))
 */
Json::Value MoDB::InsertSonComment(Json::Value &insertjson) {
    try {
        // 构造评论 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        int64_t parentid = stoll(insertjson["ParentId"].asString());
        string content = insertjson["Content"].asString();
        int64_t userid = stoll(insertjson["UserId"].asString());
        string createtime = GetTime();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        // 构造更新文档，向子评论数组中添加新评论
        bsoncxx::builder::stream::document document{};
        document << "$addToSet" << open_document << "Child_Comments" << open_document << "_id" << id << "Content"
                 << content.data() << "UserId" << userid << "Likes" << 0 << "CreateTime" << createtime.data()
                 << close_document << close_document;

        // 执行更新操作
        auto result = commentcoll.update_one({make_document(kvp("_id", parentid))}, document.view());
        // 检查更新结果
        if (!result) {
            // 数据库操作本身失败（如连接中断等）
            return response::DatabaseError("数据库操作失败！");
        }
        // 是否找到目标文档（即父评论是否存在）
        if (result->matched_count() == 0) {
            // 未找到要更新的父评论，说明父评论不存在
            return response::CommentNotFound("父评论不存在，无法添加子评论！");
        }
        // 是否实际插入了子评论
        if (result->modified_count() == 0) {
            // 子评论插入失败
            return response::CommentInsertFailed("子评论插入失败！");
        }

        // 子评论插入成功，更新对应的讨论或题解的评论数
        // 1. 构造补偿操作的删除文档
        bsoncxx::builder::stream::document pullDoc{};
        pullDoc << "$pull" << open_document << "Child_Comments" << open_document << "_id" << id << close_document
                << close_document;
        // 2. 先查询父评论的父级 ID（ParentId） 和类型（ParentType）
        mongocxx::options::find options;
        options.projection(make_document(kvp("ParentId", 1), kvp("ParentType", 1)));
        auto parentComment = commentcoll.find_one({make_document(kvp("_id", parentid))}, options);
        if (!parentComment) {
            commentcoll.update_one({make_document(kvp("_id", parentid))}, pullDoc.view());
            return response::CommentNotFound("父评论不存在，无法添加子评论！");
        }
        // 取得父评论的 ParentId 和 ParentType
        int64_t parent_articleid = parentComment->view()["ParentId"].get_int64().value;
        string parenttype = parentComment->view()["ParentType"].get_string().value.data();
        // 3. 根据父评论类型更新对应的评论数
        bool updateResult = UpdateArticleComments(parent_articleid, parenttype, 1);
        // 4. 如果更新评论数失败，执行补偿操作，删除刚插入的子评论
        if (!updateResult) {
            commentcoll.update_one({make_document(kvp("_id", parentid))}, pullDoc.view());
            return response::DatabaseError("更新评论数失败，请稍后重试！");
        }

        // 构造返回数据
        Json::Value data;
        data["CommentId"] = to_string(id);
        data["CreateTime"] = createtime;
        return response::Success("回复成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：获取父评论
 * 权限：所有用户均可查询
 * @name GetFatherComment
 * @brief 查询父评论及其部分子评论
 * @param queryjson Json(Page, PageSize, ParentId, SonNum)
 * @return Json(success, code, message, data(List[{_id, ParentId, Content, Likes, CreateTime, Child_Total, User{_id,
 * Avatar, NickName}, Child_Comments[{_id, Content, Likes, CreateTime, User{_id, Avatar, NickName}}]}], Total))
 */
Json::Value MoDB::GetFatherComment(Json::Value &queryjson) {
    try {
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        // 提取父级 ID 和子评论数量限制
        int64_t parentid = stoll(queryjson["ParentId"].asString());
        int sonnum = stoi(queryjson["SonNum"].asString());
        // 计算跳过的条数
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        // 匹配 ParentId
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
        // 将子评论的用户 ID 和用户表进行外连接
        document << "from" << COLLECTION_USERS << "localField" << "Child_Comments.UserId" << "foreignField" << "_id"
                 << "as" << "Child_Comments.User";
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
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
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
        Json::Value list(Json::arrayValue);

        mongocxx::cursor cursor = commentcoll.aggregate(pipe);
        // 检查查询结果是否为空
        if (cursor.begin() == cursor.end()) {
            return response::CommentNotFound();
        }
        // 遍历查询结果，构造返回列表
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        int total = static_cast<int>(commentcoll.count_documents({make_document(kvp("ParentId", parentid))}));
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：获取子评论
 * 权限：所有用户均可查询
 * @name GetSonComment
 * @brief 获取指定父评论的子评论列表
 * @param queryjson Json(ParentId, Page, PageSize)
 * @return Json(success, code, message, data(Child_Total, Child_Comments[{_id, Content, Likes, CreateTime, User{_id,
 * NickName, Avatar}}]))
 */
Json::Value MoDB::GetSonComment(Json::Value &queryjson) {
    try {
        // 提取要查询的父评论 ID
        int64_t _id = stoll(queryjson["ParentId"].asString());
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
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
        document << "from" << COLLECTION_USERS << "localField" << "Child_Comments.UserId" << "foreignField" << "_id"
                 << "as" << "Child_Comments.User";
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
        Json::Value data;

        mongocxx::cursor cursor = commentcoll.aggregate(pipe);
        // 检查查询结果是否为空
        if (cursor.begin() == cursor.end()) {
            return response::CommentNotFound();
        }
        // 解析查询结果
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询评论
 * 权限：只允许管理员查询
 * @name SelectCommentListByAdmin
 * @brief 分页查询评论列表（管理员权限）
 * @param queryjson Json(Page, PageSize)
 * @return Json(success, code, message, data(List[{_id, ParentId, ParentType, Content, CreateTime, Child_Total,
 * Child_Comments[{_id, Content, CreateTime}]}], Total))
 */
Json::Value MoDB::SelectCommentListByAdmin(Json::Value &queryjson) {
    try {
        // 提取搜索参数
        Json::Value searchinfo = queryjson["SearchInfo"];
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe, pipetot;

        // 查询父级类型（ParentType）
        if (searchinfo["ParentType"].asString().size() > 0) {
            pipe.match({make_document(kvp("ParentType", searchinfo["ParentType"].asString()))});
            pipetot.match({make_document(kvp("ParentType", searchinfo["ParentType"].asString()))});
        }

        // 查询用户 ID（UserId）
        if (searchinfo["UserId"].asString().size() > 0) {
            int64_t userid = stoll(searchinfo["UserId"].asString());
            pipe.match({make_document(kvp("UserId", userid))});
            pipetot.match({make_document(kvp("UserId", userid))});
        }

        // 查询评论内容（Content）
        if (searchinfo["Content"].asString().size() > 0) {
            document << "Content" << open_document << "$regex" << searchinfo["Content"].asString() << close_document;
            pipe.match(document.view());
            pipetot.match(document.view());
            document.clear();
        }

        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        mongocxx::cursor cursor = commentcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            Json::Reader reader;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        // 稳定排序：CreateTime 可能秒级重复，且后续 $group 会打乱顺序，先加 _id 作为 tie-breaker
        pipe.sort({make_document(kvp("CreateTime", -1), kvp("_id", -1))});
        // 跳过多少条
        pipe.skip(skip);
        // 限制多少条
        pipe.limit(pagesize);
        // 将子评论进行求个数
        document << "$set" << open_document << "Child_Total" << open_document << "$size" << "$Child_Comments"
                 << close_document << close_document;
        pipe.append_stage(document.view());
        document.clear();
        // 将父评论的用户Id和用户表进行外连接
        document << "from" << COLLECTION_USERS << "localField" << "UserId" << "foreignField" << "_id" << "as" << "User";
        pipe.lookup(document.view());
        document.clear();

        // 拆散子评论，逐条查询子评论用户信息后再合并回数组
        document << "path" << "$Child_Comments" << "preserveNullAndEmptyArrays" << true;
        pipe.unwind(document.view());
        document.clear();
        document << "from" << COLLECTION_USERS << "localField" << "Child_Comments.UserId" << "foreignField" << "_id"
                 << "as" << "Child_Comments.User";
        pipe.lookup(document.view());
        document.clear();

        document << "_id" << "$_id" << "ParentId" << open_document << "$first" << "$ParentId" << close_document
                 << "ParentType" << open_document << "$first" << "$ParentType" << close_document << "Content"
                 << open_document << "$first" << "$Content" << close_document << "Likes" << open_document << "$first"
                 << "$Likes" << close_document << "CreateTime" << open_document << "$first" << "$CreateTime"
                 << close_document << "Child_Total" << open_document << "$first" << "$Child_Total" << close_document
                 << "User" << open_document << "$first" << "$User" << close_document << "Child_Comments"
                 << open_document << "$push" << "$Child_Comments" << close_document;
        pipe.group(document.view());
        document.clear();

        // $group 之后顺序不保证稳定，重新排序以确保分页结果一致
        pipe.sort({make_document(kvp("CreateTime", -1), kvp("_id", -1))});

        // 选择需要的字段
        document << "ParentId" << 1 << "ParentType" << 1 << "Content" << 1 << "Likes" << 1 << "User._id" << 1
                 << "User.Avatar" << 1 << "User.NickName" << 1 << "CreateTime" << 1 << "Child_Total" << 1
                 << "Child_Comments._id" << 1 << "Child_Comments.Content" << 1 << "Child_Comments.User._id" << 1
                 << "Child_Comments.User.NickName" << 1 << "Child_Comments.User.Avatar" << 1
                 << "Child_Comments.CreateTime" << 1;
        pipe.project(document.view());
        document.clear();

        Json::Reader reader;
        Json::Value list(Json::arrayValue);

        cursor = commentcoll.aggregate(pipe);
        // 检查查询结果是否为空
        if (cursor.begin() == cursor.end()) {
            return response::CommentNotFound();
        }
        // 遍历查询结果，构造返回列表
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        // int total = static_cast<int>(commentcoll.count_documents({}));
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        // TODO: 记录日志 e.what()
        cout << "SelectCommentListByAdmin error: " << e.what() << endl;
        return response::DatabaseError();
    }
}

/**
 * 功能：删除父评论
 * 权限：只允许评论作者本人或者管理员删除
 * @name DeleteFatherComment
 * @brief 删除父评论
 * @param deletejson Json(CommentId)
 * @return Json(success, code, message, data(Result))
 * @note 删除父评论时会同时删除所有子评论，并更新对应的评论数
 */
Json::Value MoDB::DeleteFatherComment(Json::Value &deletejson) {
    try {
        // 获取评论 ID
        int64_t commentid = stoll(deletejson["CommentId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        // 查询父评论是否存在：使用 find 查询父评论
        mongocxx::cursor cursor = commentcoll.find({make_document(kvp("_id", commentid))});
        // 如果未查询到父评论
        if (cursor.begin() == cursor.end()) {
            return response::CommentNotFound();
        }

        // 使用聚合管道查询子评论数量
        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        pipe.match({make_document(kvp("_id", commentid))});
        document << "$set" << open_document << "Child_Total" << open_document << "$size" << "$Child_Comments"
                 << close_document << close_document;
        pipe.append_stage(document.view());
        cursor = commentcoll.aggregate(pipe);

        // 解析查询结果
        Json::Value jsonvalue;
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
        }

        // 获取子评论数量和父评论类型
        int sonnum = stoi(jsonvalue["Child_Total"].asString());
        string articletype = jsonvalue["ParentType"].asString();
        int64_t parentid = stoll(jsonvalue["ParentId"].asString());

        /**
         * 分布式事务一致性问题：确保评论删除和对应的评论数更新的一致性
         * 方案：补偿机制（最终一致性）
         * 采用"先更新评论数，成功后在删除评论，若删除评论失败则补偿回滚"的策略
         * 步骤：
         * 1. 先更新对应的评论数，减少评论数
         * 2. 如果更新评论数成功，尝试删除评论
         * 3. 如果删除评论失败，执行补偿操作，恢复刚才减少的评论数
         * 4. 返回错误信息，提示用户稍后重试
         */

        // 1. 先更新对应的评论数，减少评论数（包括父评论和子评论）
        int deleteNum = -(sonnum + 1);
        bool updateResult = UpdateArticleComments(parentid, articletype, deleteNum);
        // 如果更新评论数失败，返回错误信息
        if (!updateResult) {
            return response::DatabaseError("更新评论数失败，请稍后重试！");
        }

        // 2. 如果更新评论数成功，尝试删除父评论
        auto result = commentcoll.delete_one({make_document(kvp("_id", commentid))});
        // 检查删除结果
        if ((*result).deleted_count() < 1) {
            // 3. 如果删除评论失败，执行补偿操作，恢复刚才减少的评论数
            UpdateArticleComments(parentid, articletype, -deleteNum);
            // 4. 返回错误信息，提示用户稍后重试
            return response::CommentNotFound("评论删除失败，请稍后重试！");
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->deleted_count());
        return response::Success("删除评论成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除子评论
 * 权限：只允许评论作者本人或者管理员删除
 * @name DeleteSonComment
 * @brief 删除子评论
 * @param deletejson Json(CommentId)
 * @return Json(success, code, message, data(Result))
 */
Json::Value MoDB::DeleteSonComment(Json::Value &deletejson) {
    try {
        // 获取评论 ID
        int64_t commentid = stoll(deletejson["CommentId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        // 找出父评论数据
        mongocxx::cursor cursor = commentcoll.find({make_document(kvp("Child_Comments._id", commentid))});
        if (cursor.begin() == cursor.end()) {
            return response::CommentNotFound();
        }

        // 解析查询结果
        Json::Value jsonvalue;
        Json::Reader reader;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
        }
        // 提取父评论 ID(_id) 和父评论的父级 ID(ParentId) 及类型 (ParentType)
        int64_t father_commentid = stoll(jsonvalue["_id"].asString());
        int64_t parentid = stoll(jsonvalue["ParentId"].asString());
        string articletype = jsonvalue["ParentType"].asString();

        // 先更新对应的评论数，减少评论数（只减少子评论数）
        bool updateResult = UpdateArticleComments(parentid, articletype, -1);
        // 如果更新评论数失败，返回错误信息
        if (!updateResult) {
            return response::DatabaseError("更新评论数失败，请稍后重试！");
        }

        // 删除子评论
        bsoncxx::builder::stream::document document{};
        document << "$pull" << open_document << "Child_Comments" << open_document << "_id" << commentid
                 << close_document << close_document;

        // 执行更新操作
        auto result = commentcoll.update_one({make_document(kvp("_id", father_commentid))}, document.view());
        // 检查更新结果
        if (!result) {
            // 数据库操作本身失败（如连接中断等）
            // 恢复刚才减少的评论数
            UpdateArticleComments(parentid, articletype, 1);
            return response::DatabaseError("数据库操作失败！");
        }
        // 是否找到目标文档（即父评论是否存在）
        if (result->matched_count() == 0) {
            // 未找到要更新的父评论，说明父评论不存在
            // 恢复刚才减少的评论数
            UpdateArticleComments(parentid, articletype, 1);
            return response::CommentNotFound("父评论不存在，无法删除子评论！");
        }
        // 是否实际删除了子评论
        if (result->modified_count() == 0) {
            // 子评论删除失败
            // 恢复刚才减少的评论数
            UpdateArticleComments(parentid, articletype, 1);
            return response::CommentNotFound("子评论不存在，删除失败！");
        }

        // 构造返回数据
        Json::Value data;
        data["Result"] = static_cast<bool>(result->modified_count());
        return response::Success("删除评论成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：删除某一篇文章（讨论，题解，公告）的所有评论，主要服务于删除文章
 * @name DeleteArticleComment
 * @brief 删除某一篇文章（讨论，题解，公告）的所有评论
 * @param deletejson Json(ArticleId)
 * @return bool 删除是否成功
 */
bool MoDB::DeleteArticleComment(Json::Value &deletejson) {
    try {
        // 获取文章 ID
        int64_t articleid = stoll(deletejson["ArticleId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];
        // 删除所有与该文章相关的评论
        auto result = commentcoll.delete_many({make_document(kvp("ParentId", articleid))});
        // 判断删除是否成功
        if (!result) {
            return false;
        }
        if (result->deleted_count() == 0) {
            return false;
        }

        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

/**
 * 功能：获取评论的作者 UserId
 * @name GetCommentAuthorId
 * @brief 获取评论的作者 UserId
 * @param commentId 评论 ID
 * @return 作者 UserId，如果不存在返回空字符串
 */
std::string MoDB::GetCommentAuthorId(int64_t commentId) {
    try {
        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection commentcoll = (*client)[DATABASE_NAME][COLLECTION_COMMENTS];

        // 先查询父评论
        // 构造查询条件
        bsoncxx::builder::stream::document filterBuilder{};
        filterBuilder << "_id" << commentId;

        // 构造 projection：只返回 UserId 字段
        bsoncxx::builder::stream::document projection{};
        projection << "UserId" << 1;

        // 设置查询选项
        mongocxx::options::find options;
        options.projection(projection.view());

        // 执行查询
        auto result = commentcoll.find_one(filterBuilder.view(), options);

        // 处理查询结果
        if (result) {
            auto view = result->view();
            auto userid_element = view["UserId"];
            if (userid_element) {
                return std::to_string(userid_element.get_int64().value);
            }
        }

        // 如果不是父评论，查询子评论
        mongocxx::cursor cursor = commentcoll.find({make_document(kvp("Child_Comments._id", commentId))});
        Json::Reader reader;
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            // 遍历子评论找到匹配的
            if (jsonvalue["Child_Comments"].isArray()) {
                for (const auto &child : jsonvalue["Child_Comments"]) {
                    if (child["_id"].asInt64() == commentId) {
                        return child["UserId"].asString();
                    }
                }
            }
        }
        return "";
    } catch (const std::exception &e) {
        return "";
    }
}
// ------------------------------ 评论模块 End ------------------------------

// ------------------------------ 测评记录模块 Start ------------------------------
/**
 * 功能：将字符串变为 int64
 * 主要为查询 ID 服务，限制：ID 长度不能大于 19，只关注数字
 */
int64_t mystoll(string num) {
    int64_t resnum = 0;
    if (num.size() >= 19 || num.size() == 0)
        return resnum;
    for (auto n : num) {
        if (isdigit(n)) {
            resnum = resnum * 10 + n - '0';
        }
    }
    return resnum;
}

/**
 * 功能：查询测评记录
 * 权限：所有用户均可查询
 * @name SelectStatusRecord
 * @brief 查询指定测评记录的详细信息
 * @param queryjson Json(StatusRecordId)
 * @return Json(success, code, message, data(Status, Language, Code, CompilerInfo, TestInfo))
 */
Json::Value MoDB::SelectStatusRecord(Json::Value &queryjson) {
    try {
        // 获取测评记录 ID
        int64_t statusrecordid = stoll(queryjson["StatusRecordId"].asString());

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection statusrecordcoll = (*client)[DATABASE_NAME][COLLECTION_STATUS_RECORDS];

        // 构造聚合管道
        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        pipe.match({make_document(kvp("_id", statusrecordid))});
        document << "Status" << 1 << "Language" << 1 << "Code" << 1 << "CompilerInfo" << 1 << "TestInfo" << 1;
        pipe.project(document.view());

        // 查询测评记录
        mongocxx::cursor cursor = statusrecordcoll.aggregate(pipe);
        // 检查查询结果是否为空
        if (cursor.begin() == cursor.end()) {
            return response::StatusRecordNotFound();
        }

        // 解析查询结果
        Json::Reader reader;
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }

        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：分页查询测评记录
 * 权限：所有用户均可查询
 * @name SelectStatusRecordList
 * @brief 分页查询测评记录列表
 * @param queryjson Json(PageSize, Page, SearchInfo(可选，包含 ProblemId, UserId, ProblemTitle, Status, Language))
 * @return Json(success, code, message, data(List[{_id, ProblemId, UserId, UserNickName, ProblemTitle, Status, RunTime,
 * RunMemory, Length, Language, SubmitTime}], Total))
 */
Json::Value MoDB::SelectStatusRecordList(Json::Value &queryjson) {
    try {
        // 提取分页参数
        int page = stoi(queryjson["Page"].asString());
        int pagesize = stoi(queryjson["PageSize"].asString());
        int skip = (page - 1) * pagesize;

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection statusrecordcoll = (*client)[DATABASE_NAME][COLLECTION_STATUS_RECORDS];

        // 构造聚合管道
        mongocxx::pipeline pipe, pipetot;
        bsoncxx::builder::stream::document document{};

        // 检查是否存在 SearchInfo 参数
        if (queryjson.isMember("SearchInfo") && !queryjson["SearchInfo"].isNull()) {
            Json::Value searchinfo = queryjson["SearchInfo"];

            // 查询题目 ID
            if (searchinfo.isMember("ProblemId") && searchinfo["ProblemId"].asString().size() > 0) {
                int64_t problemid = mystoll(searchinfo["ProblemId"].asString());
                pipe.match({{make_document(kvp("ProblemId", problemid))}});
                pipetot.match({{make_document(kvp("ProblemId", problemid))}});
            }
            // 查询用户 ID
            if (searchinfo.isMember("UserId") && searchinfo["UserId"].asString().size() > 0) {
                int64_t userid = stoll(searchinfo["UserId"].asString());
                pipe.match({{make_document(kvp("UserId", userid))}});
                pipetot.match({{make_document(kvp("UserId", userid))}});
            }

            // 查询题目标题
            if (searchinfo.isMember("ProblemTitle") && searchinfo["ProblemTitle"].asString().size() > 0) {
                document << "ProblemTitle" << open_document << "$regex" << searchinfo["ProblemTitle"].asString()
                         << close_document;
                pipe.match(document.view());
                pipetot.match(document.view());
                document.clear();
            }

            // 查询状态
            if (searchinfo.isMember("Status") && searchinfo["Status"].asString().size() > 0) {
                int status = stoi(searchinfo["Status"].asString());
                pipe.match({{make_document(kvp("Status", status))}});
                pipetot.match({{make_document(kvp("Status", status))}});
            }

            // 查询语言
            if (searchinfo.isMember("Language") && searchinfo["Language"].asString().size() > 0) {
                string language = searchinfo["Language"].asString();
                pipe.match({{make_document(kvp("Language", language))}});
                pipetot.match({{make_document(kvp("Language", language))}});
            }
        }
        // 获取总条数
        int total = 0;
        pipetot.count("TotalNum");
        Json::Reader reader;
        mongocxx::cursor cursor = statusrecordcoll.aggregate(pipetot);
        for (auto doc : cursor) {
            Json::Value tmpjson;
            reader.parse(bsoncxx::to_json(doc), tmpjson);
            total = tmpjson["TotalNum"].asInt();
        }

        // 排序
        pipe.sort({make_document(kvp("SubmitTime", -1))});
        // 跳过
        pipe.skip(skip);
        // 限制
        pipe.limit(pagesize);

        document << "ProblemId" << 1 << "UserId" << 1 << "UserNickName" << 1 << "ProblemTitle" << 1 << "Status" << 1
                 << "RunTime" << 1 << "RunMemory" << 1 << "Length" << 1 << "Language" << 1 << "SubmitTime" << 1;
        pipe.project(document.view());
        Json::Value list(Json::arrayValue);
        cursor = statusrecordcoll.aggregate(pipe);

        // 遍历查询结果，构造返回列表
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }
        return response::SuccessList(list, total);
    } catch (const std::exception &e) {
        return response::DatabaseError();
    }
}

/**
 * 功能：插入待测评记录
 * @name InsertStatusRecord
 * @brief 插入新的测评记录，初始状态为等待测评
 * @param insertjson Json(ProblemId, UserId, UserNickName, ProblemTitle, Language, Code)
 * @return 测评记录 ID，如果插入失败返回 "0"
 */
string MoDB::InsertStatusRecord(Json::Value &insertjson) {
    try {
        // 构造测评记录 ID，使用雪花算法生成唯一 ID
        int64_t id = IDGenerator::Instance().NextId();
        int64_t problemid = stoll(insertjson["ProblemId"].asString());
        int64_t userid = stoll(insertjson["UserId"].asString());
        string usernickname = insertjson["UserNickName"].asString();
        string problemtitle = insertjson["ProblemTitle"].asString();
        string language = insertjson["Language"].asString();
        string code = insertjson["Code"].asString();

        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection statusrecordcoll = (*client)[DATABASE_NAME][COLLECTION_STATUS_RECORDS];
        // 插入测评记录，初始状态为 0（等待测评），运行时间、内存、代码长度均为 0，编译信息为空，测试信息为空数组
        bsoncxx::builder::stream::document document{};
        document << "_id" << id << "ProblemId" << problemid << "UserId" << userid << "UserNickName"
                 << usernickname.data() << "ProblemTitle" << problemtitle.data() << "Status"
                 << constants::judge::STATUS_PENDING_JUDGING << "RunTime" << "0MS" << "RunMemory" << "0MB" << "Length"
                 << "0B" << "Language" << language.data() << "SubmitTime" << GetTime().data() << "Code" << code.data()
                 << "CompilerInfo" << "" << "TestInfo" << open_array << close_array;
        // 插入文档
        auto result = statusrecordcoll.insert_one(document.view());
        // 检查插入结果
        if ((*result).result().inserted_count() < 1) {
            return "0";
        }
        // 返回测评记录 ID
        return to_string(id);
    } catch (const std::exception &e) {
        return "0";
    }
}

/**
 * 功能：更新测评记录
 * @name UpdateStatusRecord
 * @brief 更新指定测评记录的状态和测试信息
 * @param updatejson Json(StatusRecordId, Status, RunTime, RunMemory, Length, CompilerInfo, TestInfo[{Status,
 * StandardInput, StandardOutput, PersonalOutput, RunTime, RunMemory}])
 * @return bool 更新是否成功
 */
bool MoDB::UpdateStatusRecord(Json::Value &updatejson) {
    Json::Value resjson;
    try {
        // 获取测评记录 ID 和更新内容
        int64_t submitid = stoll(updatejson["StatusRecordId"].asString());
        int status = stoi(updatejson["Status"].asString());
        string runtime = updatejson["RunTime"].asString();
        string runmemory = updatejson["RunMemory"].asString();
        string length = updatejson["Length"].asString();
        string complierinfo = updatejson["CompilerInfo"].asString();

        // 更新测评记录
        auto client = pool.acquire();
        mongocxx::collection statusrecordcoll = (*client)[DATABASE_NAME][COLLECTION_STATUS_RECORDS];
        bsoncxx::builder::stream::document document{};
        auto in_array = document << "$set" << open_document << "Status" << status << "RunTime" << runtime.data()
                                 << "RunMemory" << runmemory.data() << "Length" << length.data() << "CompilerInfo"
                                 << complierinfo.data() << "TestInfo" << open_array;

        // 构造测试信息数组
        for (int i = 0; i < updatejson["TestInfo"].size(); i++) {
            int teststatus = stoi(updatejson["TestInfo"][i]["Status"].asString());
            string standardinput = updatejson["TestInfo"][i]["StandardInput"].asString();
            string standardoutput = updatejson["TestInfo"][i]["StandardOutput"].asString();
            string personaloutput = updatejson["TestInfo"][i]["PersonalOutput"].asString();
            string testruntime = updatejson["TestInfo"][i]["RunTime"].asString();
            string testrunmemory = updatejson["TestInfo"][i]["RunMemory"].asString();
            in_array = in_array << open_document << "Status" << teststatus << "StandardInput" << standardinput
                                << "StandardOutput" << standardoutput << "PersonalOutput" << personaloutput << "RunTime"
                                << testruntime << "RunMemory" << testrunmemory << close_document;
        }
        bsoncxx::document::value doc = in_array << close_array << close_document << finalize;

        // 执行更新操作
        auto result = statusrecordcoll.update_one({make_document(kvp("_id", submitid))}, doc.view());
        // 返回更新结果
        return static_cast<bool>(result->modified_count());
    } catch (const std::exception &e) {
        return false;
    }
}

/**
 * 功能：获取测评记录的作者 UserId
 * @name GetStatusRecordAuthorId
 * @brief 获取测评记录的作者 UserId
 * @param statusRecordId 测评记录 ID
 * @return 作者 UserId，如果不存在返回空字符串
 */
std::string MoDB::GetStatusRecordAuthorId(int64_t statusRecordId) {
    try {
        // 获取数据库连接
        auto client = pool.acquire();
        mongocxx::collection statusrecordcoll = (*client)[DATABASE_NAME][COLLECTION_STATUS_RECORDS];

        // 构造查询条件
        bsoncxx::builder::stream::document filterBuilder{};
        filterBuilder << "_id" << statusRecordId;

        // 构造 projection：只返回 UserId 字段
        bsoncxx::builder::stream::document projection{};
        projection << "UserId" << 1;

        // 设置查询选项
        mongocxx::options::find options;
        options.projection(projection.view());

        // 执行查询
        auto result = statusrecordcoll.find_one(filterBuilder.view(), options);

        // 处理查询结果
        if (!result) {
            return "";
        }

        // 解析结果视图
        auto view = result->view();
        auto userid_element = view["UserId"];

        // 类型安全检查
        if (!userid_element || userid_element.type() != bsoncxx::type::k_int64) {
            return "";
        }

        return std::to_string(userid_element.get_int64().value);
    } catch (const std::exception &e) {
        return "";
    }
}
// ------------------------------ 测评记录模块 End ------------------------------

// ------------------------------ Token 鉴权实现 Start ------------------------------
/**
 * 功能：用户登录通过 Token 鉴权
 * 权限：所有用户均可使用
 * @name LoginUserByToken
 * @brief 用户登录通过 Token 鉴权
 * @param loginjson Json(UserId)
 * @return Json(success, code, message, data(_id, NickName, Avatar, CommentLikes, Solves, Authority, Token))
 */
Json::Value MoDB::LoginUserByToken(Json::Value &loginjson) {
    try {
        string userid_str = loginjson["UserId"].asString();
        if (userid_str.empty()) {
            return response::UserTokenInvalid();
        }
        int64_t userid = stoll(userid_str);
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        mongocxx::pipeline pipe;
        bsoncxx::builder::stream::document document{};
        document << "_id" << userid;
        pipe.match(document.view());

        document.clear();
        document << "Avatar" << 1 << "NickName" << 1 << "CommentLikes" << 1 << "Solves" << 1 << "Authority" << 1;

        pipe.project(document.view());

        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        // 匹配失败
        if (cursor.begin() == cursor.end()) {
            return response::UserNotFound("用户ID错误！");
        }
        // 匹配成功
        Json::Reader reader;
        Json::Value data;
        for (auto doc : cursor) {
            reader.parse(bsoncxx::to_json(doc), data);
        }
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}

/**
 * 功能：查询所有用户的权限
 * 传入：void
 * 传出：Json(success, code, message, data(list))
 */
Json::Value MoDB::SelectUserAuthority() {
    try {
        auto client = pool.acquire();
        mongocxx::collection usercoll = (*client)[DATABASE_NAME][COLLECTION_USERS];

        bsoncxx::builder::stream::document document{};
        mongocxx::pipeline pipe;

        document << "Authority" << 1;

        pipe.project(document.view());
        Json::Reader reader;

        mongocxx::cursor cursor = usercoll.aggregate(pipe);
        Json::Value list(Json::arrayValue);
        for (auto doc : cursor) {
            Json::Value jsonvalue;
            reader.parse(bsoncxx::to_json(doc), jsonvalue);
            list.append(jsonvalue);
        }

        Json::Value data;
        data["list"] = list;
        return response::Success("查询成功", data);
    } catch (const std::exception &e) {
        return response::InternalError("数据库异常：" + std::string(e.what()));
    }
}
// ------------------------------ Token 鉴权实现 End ------------------------------

MoDB::MoDB() {
    // 构造函数实现
}

MoDB::~MoDB() {
    // 析构函数实现
}