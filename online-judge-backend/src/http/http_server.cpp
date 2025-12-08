#include "http/http_server.h"

#include <httplib/httplib.h>  // 使用 httplib 作为 HTTP 服务器库
#include <json/json.h>        // 使用 JsonCpp 处理 JSON 数据

#include <fstream>  // C++17 文件系统库
#include <iostream>
#include <set>
#include <string>
#include <typeinfo>

#include "constants/error_code.h"
#include "constants/server.h"
#include "core/control.h"
#include "services/user_service.h"  // 用户服务（用于登录验证）
#include "utils/json_utils.h"       // JSON 工具
#include "utils/response.h"         // 统一响应工具

using namespace std;

Control control;  // 全局 Control 对象

/**
 * 根据返回结果设置 HTTP 状态码
 * 新的响应结构：{ success, code, message, data }
 */
void SetResponseStatus(const Json::Value &json, httplib::Response &res) {
    // 根据 code 设置 HTTP 状态码
    int code = json["code"].asInt();
    switch (code) {
        case error_code::SUCCESS:
            res.status = 200;
            break;
        case error_code::BAD_REQUEST:
            res.status = 400;
            break;
        case error_code::UNAUTHORIZED:
        case error_code::USER_TOKEN_INVALID:
            res.status = 401;
            break;
        case error_code::FORBIDDEN:
            res.status = 403;
            break;
        case error_code::NOT_FOUND:
        case error_code::USER_NOT_FOUND:
        case error_code::PROBLEM_NOT_FOUND:
        case error_code::ANNOUNCEMENT_NOT_FOUND:
        case error_code::DISCUSS_NOT_FOUND:
        case error_code::SOLUTION_NOT_FOUND:
        case error_code::COMMENT_NOT_FOUND:
        case error_code::STATUS_RECORD_NOT_FOUND:
            res.status = 404;
            break;
        case error_code::INTERNAL_ERROR:
        case error_code::DATABASE_ERROR:
            res.status = 500;
            break;
        default:
            // 其他业务错误码，HTTP 状态码仍为 200
            res.status = 200;
            break;
    }
}

// 获取请求中的 Token
string GetRequestToken(const httplib::Request &req) {
    auto res = req.headers.find("Authorization");
    if (res != req.headers.end()) {
        return res->second;
    } else {
        return "";
    }
}

// ------------------------------ 用户模块 Start ------------------------------
/**
 * 处理注册用户的请求
 */
void doUserRegister(const httplib::Request &req, httplib::Response &res) {
    cout << "doUserRegister start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UserRegister(jsonvalue);
    cout << "doUserRegister end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理登录用户的请求
 */
void doUserLogin(const httplib::Request &req, httplib::Response &res) {
    cout << "doUserLogin start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UserLogin(jsonvalue);
    cout << "doUserLogin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取用户信息的请求
 */
void doGetUserInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserInfo start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取用户 ID 参数
        string userid = req.get_param_value("UserId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserInfo(queryjson);
    }
    cout << "doGetUserInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理设置页面获取用户信息的请求
 */
void doGetUserUpdateInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserUpdateInfo start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson = response::BadRequest();
    } else {
        // 获取用户 ID 参数
        string userid = req.get_param_value("UserId");
        // 获取 Token 参数
        string token = GetRequestToken(req);
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserUpdateInfo(queryjson);
    }
    cout << "doGetUserUpdateInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理更新用户信息的请求
 */
void doUpdateUserInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateUserInfo start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    // 解析传入的 Json
    Json::Value jsonvalue;
    Json::Reader reader;
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.UpdateUserInfo(jsonvalue);
    cout << "doUpdateUserInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除用户的请求
 */
void doDeleteUser(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteUser start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取用户 ID 参数
        string userid = req.get_param_value("UserId");
        Json::Value jsonvalue;
        jsonvalue["Token"] = token;
        jsonvalue["UserId"] = userid;
        resjson = control.DeleteUser(jsonvalue);
    }
    cout << "doDeleteUser end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取用户排名的请求
 */
void doGetUserRank(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserRank start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserRank(queryjson);
    }
    cout << "doGetUserRank end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取用户列表的请求（管理员权限）
 */
void doGetUserSetInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserSetInfo start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserSetInfo(queryjson);
    }
    cout << "doGetUserSetInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

// 通过 Token 登录用户获取信息
void doGetUserInfoByToken(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserInfoByToken start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value queryjson;
    queryjson["Token"] = token;
    Json::Value resjson = control.LoginUserByToken(queryjson);
    cout << "doGetUserInfoByToken end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

// 处理用户修改密码的请求
void doUpdateUserPassword(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateUserPassword start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    // 解析传入的 Json
    Json::Value updatejson;
    Json::Reader reader;
    reader.parse(req.body, updatejson);
    updatejson["Token"] = token;
    Json::Value resjson = control.UpdateUserPassword(updatejson);
    cout << "doUpdateUserPassword end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理用户退出登录的请求
 */
void doUserLogout(const httplib::Request &req, httplib::Response &res) {
    cout << "doUserLogout start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value logoutjson;
    logoutjson["Token"] = token;
    Json::Value resjson = control.UserLogout(logoutjson);
    cout << "doUserLogout end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
/**
 * 处理获取单条题目信息的请求
 */
void doGetProblemInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemInfo start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("ProblemId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题目 ID 参数
        string problemid = req.get_param_value("ProblemId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblemInfo(queryjson);
    }
    cout << "doGetProblemInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取单条题目信息的请求（管理员权限）
 */
void doGetProblemInfoByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemInfoByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("ProblemId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题目 ID 参数
        string problemid = req.get_param_value("ProblemId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblemInfoByAdmin(queryjson);
    }
    cout << "doGetProblemInfoByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理编辑题目的请求：包含插入和更新题目（管理员权限）
 */
void doEditProblem(const httplib::Request &req, httplib::Response &res) {
    cout << "doEditProblem start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.EditProblem(jsonvalue);
    cout << "doEditProblem end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除题目的请求（管理员权限）
 */
void doDeleteProblem(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteProblem start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("ProblemId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题目 ID 参数
        string problemid = req.get_param_value("ProblemId");
        Json::Value deletejson;
        deletejson["Token"] = token;
        deletejson["ProblemId"] = problemid;
        resjson = control.DeleteProblem(deletejson);
    }
    cout << "doDeleteProblem end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取题目列表的请求
 */
void doGetProblemList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验（Page 和 PageSize 是必须的，SearchInfo 是可选的）
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        // 如果有搜索信息参数，则解析它
        if (req.has_param("SearchInfo")) {
            Json::Value searchinfo;
            Json::Reader reader;
            // 获取搜索信息参数并解析
            reader.parse(req.get_param_value("SearchInfo"), searchinfo);
            queryjson["SearchInfo"] = searchinfo;
        }
        resjson = control.SelectProblemList(queryjson);
    }
    cout << "doGetProblemList end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取题目列表的请求（管理员权限）
 */
void doGetProblemListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectProblemListByAdmin(queryjson);
    }
    cout << "doGetProblemListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 题目模块 End ------------------------------

// ------------------------------ 标签模块 Start ------------------------------
// 处理获取题目的所有标签的请求
void doGetTags(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetTags start!!!" << endl;
    Json::Value queryjson;
    string tagtype = req.get_param_value("TagType");
    queryjson["TagType"] = tagtype;
    Json::Value resjson = control.GetTags(queryjson);
    cout << "doGetTags end!!!" << endl;
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 标签模块 End ------------------------------

// ------------------------------ 公告模块 Start ------------------------------
/**
 * 处理添加公告的请求（管理员权限）
 */
void doInsertAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertAnnouncement start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.InsertAnnouncement(jsonvalue);
    cout << "doInsertAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取单条公告信息的请求，并将其浏览量加 1
 */
void doGetAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncement start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("AnnouncementId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取公告 ID 参数
        string announcementid = req.get_param_value("AnnouncementId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["AnnouncementId"] = announcementid;
        resjson = control.SelectAnnouncement(queryjson);
    }
    cout << "doGetAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理查询公告的详细信息，主要是编辑时的查询
 */
void doSelectAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectAnnouncement start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("AnnouncementId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取公告 ID 参数
        string announcementid = req.get_param_value("AnnouncementId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["AnnouncementId"] = announcementid;
        resjson = control.SelectAnnouncementByEdit(queryjson);
    }
    cout << "doSelectAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理更新公告的请求（管理员权限）
 */
void doUpdateAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateAnnouncement start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.UpdateAnnouncement(jsonvalue);
    cout << "doUpdateAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除公告的请求（管理员权限）
 */
void doDeleteAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteAnnouncement start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("AnnouncementId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取公告 ID 参数
        string announcementid = req.get_param_value("AnnouncementId");
        Json::Value deletejson;
        deletejson["Token"] = token;
        deletejson["AnnouncementId"] = announcementid;
        resjson = control.DeleteAnnouncement(deletejson);
    }
    cout << "doDeleteAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取公告列表的请求
 */
void doGetAnnouncementList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncementList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectAnnouncementList(queryjson);
    }
    cout << "doGetAnnouncementList end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取公告列表的请求（管理员权限）
 */
void doGetAnnouncementListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncementListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectAnnouncementListByAdmin(queryjson);
    }
    cout << "doGetAnnouncementListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
/**
 * 处理插入讨论的请求
 */
void doInsertDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertDiscuss start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.InsertDiscuss(jsonvalue);
    cout << "doInsertDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取单条讨论信息的请求
 */
void doGetDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscuss start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("DiscussId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取讨论 ID 参数
        string discussid = req.get_param_value("DiscussId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["DiscussId"] = discussid;
        resjson = control.SelectDiscuss(queryjson);
    }
    cout << "doGetDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理查询讨论的详细信息，主要是编辑时的查询
 */
void doSelectDiscussByEdit(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectDiscussByEdit start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("DiscussId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取讨论 ID 参数
        string discussid = req.get_param_value("DiscussId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["DiscussId"] = discussid;
        resjson = control.SelectDiscussByEdit(queryjson);
    }
    cout << "doSelectDiscussByEdit end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理更新讨论的请求
 */
void doUpdateDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateDiscuss start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.UpdateDiscuss(jsonvalue);
    cout << "doUpdateDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除讨论的请求
 */
void doDeleteDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteDiscuss start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("DiscussId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取讨论 ID 参数
        string discussid = req.get_param_value("DiscussId");
        Json::Value deletejson;
        deletejson["Token"] = token;
        deletejson["DiscussId"] = discussid;
        resjson = control.DeleteDiscuss(deletejson);
    }
    cout << "doDeleteDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取讨论列表的请求
 */
void doGetDiscussList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscussList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验（Page 和 PageSize 是必须的，SearchInfo 是可选的）
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        // 如果有搜索信息参数，则解析它
        if (req.has_param("SearchInfo")) {
            Json::Value searchinfo;
            Json::Reader reader;
            // 获取搜索信息参数并解析
            reader.parse(req.get_param_value("SearchInfo"), searchinfo);
            queryjson["SearchInfo"] = searchinfo;
        }
        resjson = control.SelectDiscussList(queryjson);
    }
    cout << "doGetDiscussList end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取讨论列表的请求（管理员权限）
 */
void doGetDiscussListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscussListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectDiscussListByAdmin(queryjson);
    }
    cout << "doGetDiscussListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
/**
 * 处理插入题解的请求
 */
void doInsertSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertSolution start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.InsertSolution(jsonvalue);
    cout << "doInsertSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取单条题解信息的请求
 */
void doGetSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolution start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("SolutionId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题解 ID 参数
        string solutionid = req.get_param_value("SolutionId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["SolutionId"] = solutionid;
        resjson = control.SelectSolution(queryjson);
    }
    cout << "doGetSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理查询题解的详细信息，主要是编辑时的查询
 */
void doSelectSolutionByEdit(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectSolutionByEdit start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("SolutionId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题解 ID 参数
        string solutionid = req.get_param_value("SolutionId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["SolutionId"] = solutionid;
        resjson = control.SelectSolutionByEdit(queryjson);
    }
    cout << "doSelectSolutionByEdit end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理更新题解的请求
 */
void doUpdateSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateSolution start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.UpdateSolution(jsonvalue);
    cout << "doUpdateSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除题解的请求
 */
void doDeleteSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteSolution start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("SolutionId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取题解 ID 参数
        string solutionid = req.get_param_value("SolutionId");
        Json::Value deletejson;
        deletejson["Token"] = token;
        deletejson["SolutionId"] = solutionid;
        resjson = control.DeleteSolution(deletejson);
    }
    cout << "doDeleteSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取题解列表的请求
 */
void doGetSolutionList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolutionList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验（Page 和 PageSize 是必须的，SearchInfo 是可选的）
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        // 如果有搜索信息参数，则解析它
        if (req.has_param("SearchInfo")) {
            Json::Value searchinfo;
            Json::Reader reader;
            // 获取搜索信息参数并解析
            reader.parse(req.get_param_value("SearchInfo"), searchinfo);
            queryjson["SearchInfo"] = searchinfo;
        }
        resjson = control.SelectSolutionList(queryjson);
    }
    cout << "doGetSolutionList end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取题解列表的请求
 */
void doGetSolutionListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolutionListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectSolutionListByAdmin(queryjson);
    }
    cout << "doGetSolutionListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

// ------------------------------ 题解模块 End ------------------------------

// ------------------------------ 评论模块 Start ------------------------------
/**
 * 处理插入评论的请求
 */
void doInsertComment(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertComment start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.InsertComment(jsonvalue);
    cout << "doInsertComment end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取评论列表的请求
 */
void doGetComment(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetComment start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("CommentType") || !req.has_param("ParentId") || !req.has_param("Page") ||
        !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        // 获取评论类型和父级 ID 参数
        string parentid = req.get_param_value("ParentId");
        string commenttype = req.get_param_value("CommentType");
        // 构造查询 Json
        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        queryjson["ParentId"] = parentid;
        queryjson["CommentType"] = commenttype;
        // 如果是查询父评论，还需要传入子评论的数量参数
        if (commenttype == "Father") {
            string sonnum = req.get_param_value("SonNum");
            if (sonnum.empty()) {
                // sonum 必须大于 0，这里默认设置为 2
                sonnum = "2";
            }
            queryjson["SonNum"] = sonnum;
        }
        resjson = control.GetComment(queryjson);
    }

    cout << "doGetComment end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理分页获取评论列表的请求（管理员权限）
 */
void doGetCommentListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetCommentListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectCommentListByAdmin(queryjson);
    }
    cout << "doGetCommentListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理删除评论的请求
 */
void doDeleteComment(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteComment start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("CommentId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取评论 ID 参数
        string commentid = req.get_param_value("CommentId");
        Json::Value deletejson;
        deletejson["Token"] = token;
        deletejson["CommentId"] = commentid;
        resjson = control.DeleteComment(deletejson);
    }
    cout << "doDeleteComment end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 评论模块 End ------------------------------

// ------------------------------ 测评记录模块 Start ------------------------------
/**
 * 处理分页获取测评记录列表的请求
 */
void doGetStatusRecordList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetStatusRecordList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验（Page 和 PageSize 是必须的，SearchInfo 可选的）
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取分页参数
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        // 构造查询 Json
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        // 如果有搜索信息参数，则解析它
        if (req.has_param("SearchInfo")) {
            Json::Value searchinfo;
            Json::Reader reader;
            // 获取搜索信息参数并解析
            reader.parse(req.get_param_value("SearchInfo"), searchinfo);
            queryjson["SearchInfo"] = searchinfo;
        }
        resjson = control.SelectStatusRecordList(queryjson);
    }
    cout << "doGetStatusRecordList end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}

/**
 * 处理获取单条测评记录的请求
 */
void doGetStatusRecord(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetStatusRecord start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("StatusRecordId")) {
        resjson = response::BadRequest();
    } else {
        // 获取 Token 参数
        string token = GetRequestToken(req);
        // 获取提交 ID 参数
        string submitid = req.get_param_value("StatusRecordId");
        Json::Value queryjson;
        queryjson["Token"] = token;
        queryjson["StatusRecordId"] = submitid;
        resjson = control.SelectStatusRecord(queryjson);
    }
    cout << "doGetStatusRecord end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 测评记录模块 End ------------------------------

// ------------------------------ 判题模块 Start ------------------------------
/**
 * 处理提交代码进行判题的请求
 */
void doJudgeCode(const httplib::Request &req, httplib::Response &res) {
    cout << "doJudgeCode start!!!" << endl;
    // 获取 Token 参数
    string token = GetRequestToken(req);
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的 Json
    reader.parse(req.body, jsonvalue);
    jsonvalue["Token"] = token;
    Json::Value resjson = control.GetJudgeCode(jsonvalue);
    cout << "doJudgeCode end!!!" << endl;
    SetResponseStatus(resjson, res);
    string resbody = JsonUtils::GetInstance()->JsonToString(resjson);
    res.set_content(resbody, "application/json; charset=utf-8");
}
// ------------------------------ 判题模块 End ------------------------------

// ------------------------------ 图片模块 Start ------------------------------
/**
 * 处理获取图片的请求
 */
void doGetImage(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetImage start!!!" << endl;

    try {
        // 请求参数校验
        if (req.matches.size() < 2) {
            res.status = 400;  // Bad Request
            res.set_content("Invalid request parameters", "text/plain");
            cout << "doGetImage Failed: Invalid request parameters" << endl;
            return;
        }
        // 获取图片索引
        int image_index = stoi(req.matches[1]);

        // 验证图片索引范围
        if (image_index < 1 || image_index > 5) {
            res.status = 400;  // Bad Request
            res.set_content("Image index out of range", "text/plain");
            cout << "doGetImage Failed: Image index out of range" << endl;
            return;
        }

        // 构造图片路径
        string imagePath = string(constants::server::AVATAR_PATH) + "/avatar" + to_string(image_index) + ".png";

        cout << "Image path: " << imagePath << endl;
        // 读取图片文件
        ifstream infile;
        infile.open(imagePath.data(), ios::binary);
        // 检查文件是否成功打开
        if (!infile.is_open()) {
            res.status = 404;  // Not Found
            res.set_content("Image not found", "text/plain");
            cout << "doGetImage Failed: Image not found" << endl;
            return;
        }
        // 读取文件内容到字符串
        string image((istreambuf_iterator<char>(infile)), (istreambuf_iterator<char>()));
        infile.close();
        res.set_content(image, "image/png");
    } catch (const exception &e) {
        res.status = 500;  // Internal Server Error
        res.set_content("Error processing request: " + string(e.what()), "text/plain");
        cerr << "Error in doGetImage: " << e.what() << endl;
    }

    cout << "doGetImage end!!!" << endl;
}
// ------------------------------ 图片模块 End ------------------------------

// ==================== 公开接口白名单（无需登录即可访问） ====================
// API 前缀
const string API = constants::server::API_PREFIX;
// 使用 set 存储，查询时间复杂度为 O(log n)
const std::set<std::string> PUBLIC_API_WHITELIST = {
    // 用户模块
    API + "/user/register",  // 用户注册
    API + "/user/login",     // 用户登录
    API + "/user/rank",      // 用户排名（公开）

    // 题目模块
    API + "/problem/info",  // 查看题目详情（公开）
    API + "/problem/list",  // 题目列表（公开）

    // 标签模块
    API + "/tags",  // 获取所有标签（公开）
    // 公告模块
    API + "/announcement/info",  // 查看公告详情（公开）
    API + "/announcement/list",  // 公告列表（公开）

    // 讨论模块
    API + "/discussion/info",  // 查看讨论详情（公开）
    API + "/discussion/list",  // 讨论列表（公开）

    // 题解模块
    API + "/solution/info",  // 查看题解详情（公开）
    API + "/solution/list",  // 题解列表（公开）
    // 评论模块
    API + "/comment/info",  // 查看评论（公开）

    // 测评记录模块
    API + "/status/record/list",  // 测评记录列表（公开）

    // 图片模块（使用前缀匹配，这里不包含）
};

/**
 * 检查请求路径是否在公开接口白名单中
 * @param path 请求路径
 * @return 如果在白名单中返回 true，否则返回 false
 */
bool IsPublicApi(const std::string &path) {
    // 精确匹配
    if (PUBLIC_API_WHITELIST.find(path) != PUBLIC_API_WHITELIST.end()) {
        return true;
    }
    // 图片接口使用前缀匹配：/api/image/xxx
    if (path.find("/api/image/") == 0) {
        return true;
    }
    // OPTIONS 预检请求放行
    return false;
}

/**
 * 运行 HTTP 服务器
 */
void HttpServer::Run() {
    using namespace httplib;
    Server server;

    // ==================== 服务器配置 Start ====================
    // 设置工作线程数
    server.new_task_queue = [] { return new ThreadPool(constants::server::MAX_THREAD_COUNT); };

    // 设置请求体大小限制
    server.set_payload_max_length(constants::server::MAX_REQUEST_BODY_SIZE);

    // 设置读取超时时间
    server.set_read_timeout(constants::server::REQUEST_TIMEOUT_SECONDS, 0);
    server.set_write_timeout(constants::server::REQUEST_TIMEOUT_SECONDS, 0);

    // 设置全局请求前处理器（登录验证中间件）
    server.set_pre_routing_handler([](const Request &req, Response &res) {
        // // 设置 CORS 跨域支持
        // res.set_header("Access-Control-Allow-Origin", "*");
        // res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        // res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Token");

        // OPTIONS 预检请求直接放行
        // if (req.method == "OPTIONS") {
        //     res.status = 204;
        //     return Server::HandlerResponse::Handled;
        // }

        // 检查是否是公开接口（白名单）
        if (IsPublicApi(req.path)) {
            // 公开接口，无需登录验证，继续处理
            return Server::HandlerResponse::Unhandled;
        }

        // 需要登录的接口，验证 Token
        string token = GetRequestToken(req);
        // 调用 UserService 检查登录状态
        Json::Value loginCheck = UserService::GetInstance()->CheckLoginByToken(token);

        // 如果返回非空 Json，说明登录验证失败
        if (!loginCheck.isNull()) {
            // 设置 HTTP 状态码
            int code = loginCheck["code"].asInt();
            if (code == error_code::UNAUTHORIZED) {
                res.status = 401;
            } else if (code == error_code::USER_TOKEN_INVALID) {
                res.status = 401;
            } else {
                res.status = 401;
            }
            // 设置响应体
            string resbody = JsonUtils::GetInstance()->JsonToString(loginCheck);
            res.set_content(resbody, "application/json; charset=utf-8");
            // 返回 Handled 表示请求已处理完毕，不再继续路由
            return Server::HandlerResponse::Handled;
        }

        // 登录验证通过，继续处理请求
        return Server::HandlerResponse::Unhandled;
    });

    // 设置全局异常处理器
    server.set_exception_handler([](const Request &req, Response &res, std::exception_ptr ep) {
        try {
            std::rethrow_exception(ep);
        } catch (const std::exception &e) {
            cerr << "[ERROR] Exception caught at " << req.method << " " << req.path << ": " << e.what() << endl;
            Json::Value error = response::InternalError("服务器内部错误：" + std::string(e.what()));
            res.status = 500;
            string errorbody = JsonUtils::GetInstance()->JsonToString(error);
            res.set_content(errorbody, "application/json; charset=utf-8");
        } catch (...) {
            cerr << "[ERROR] Unknown exception at " << req.method << " " << req.path << endl;
            Json::Value error = response::InternalError("服务器发生未知错误");
            res.status = 500;
            string errorbody = JsonUtils::GetInstance()->JsonToString(error);
            res.set_content(errorbody, "application/json; charset=utf-8");
        }
    });

    /**
     * 错误处理器，用于生成统一的错误响应格式
     * httplib 在响应状态码为错误码时会调用此处理器，即使 pre_routing_handler 返回了 Handled
     * 因此需要检查响应体是否已经设置，避免覆盖
     * @param req HTTP 请求对象
     * @param res HTTP 响应对象
     */
    server.set_error_handler([](const Request &req, Response &res) {
        // 如果响应体已经设置（如在 pre_routing_handler 中设置），则不覆盖
        if (!res.body.empty()) {
            return;
        }

        // 根据状态码生成相应的错误响应
        Json::Value error;
        switch (res.status) {
            case 400:
                error = response::BadRequest("请求参数有误");
                break;
            case 401:
                error = response::Unauthorized("未登录或登录已过期");
                break;
            case 403:
                error = response::Forbidden("无权限访问该资源");
                break;
            case 404:
                error = response::NotFound("请求的资源不存在");
                break;
            case 405:
                error = response::Fail(error_code::BAD_REQUEST, "请求方法不允许");
                break;
            case 408:
                error = response::Fail(error_code::INTERNAL_ERROR, "请求超时");
                break;
            case 413:
                error = response::Fail(error_code::BAD_REQUEST, "请求体过大");
                break;
            case 429:
                error = response::Fail(error_code::RATE_LIMIT, "请求过于频繁，请稍后再试");
                break;
            case 500:
                error = response::InternalError("服务器内部错误");
                break;
            case 502:
                error = response::Fail(error_code::INTERNAL_ERROR, "网关错误");
                break;
            case 503:
                error = response::Fail(error_code::INTERNAL_ERROR, "服务暂时不可用");
                break;
            default:
                error = response::Fail(error_code::UNKNOWN_ERROR, "请求处理失败，错误码：" + to_string(res.status));
                break;
        }
        string errorbody = JsonUtils::GetInstance()->JsonToString(error);
        res.set_content(errorbody, "application/json; charset=utf-8");
    });

    // 设置请求日志（可选，调试时使用）
    server.set_logger([](const Request &req, const Response &res) {
        cout << "[" << req.method << "] " << req.path << " -> " << res.status << endl;
    });
    // ==================== 服务器配置 End ====================

    // 设置路由和处理函数
    // -------------------- 用户模块 Start --------------------
    // 注册用户
    server.Post(API + "/user/register", doUserRegister);
    // 用户登录
    server.Post(API + "/user/login", doUserLogin);
    // 查询用户信息
    server.Get(API + "/user/info", doGetUserInfo);
    // 查询用户信息（在设置页面修改用户时使用）
    server.Get(API + "/user/select/info", doGetUserUpdateInfo);
    // 更新用户信息
    server.Post(API + "/user/update", doUpdateUserInfo);
    // 删除用户（管理员权限）
    server.Delete(API + "/admin/user/delete", doDeleteUser);
    // 用户排名查询
    server.Get(API + "/user/rank", doGetUserRank);
    // 分页查询用户列表（管理员权限）
    server.Get(API + "/admin/user/list", doGetUserSetInfo);
    // 用户登录通过 Token 鉴权（Token 鉴权实现）
    server.Get(API + "/user/auth", doGetUserInfoByToken);
    // 用户修改密码
    server.Put(API + "/user/password", doUpdateUserPassword);
    // 用户退出登录
    server.Post(API + "/user/logout", doUserLogout);
    // -------------------- 用户模块 End --------------------

    // -------------------- 题目模块 Start --------------------
    // 查询题目信息（单条）
    server.Get(API + "/problem/info", doGetProblemInfo);
    // 查询题目信息（管理员权限）
    server.Get(API + "/admin/problem/info", doGetProblemInfoByAdmin);
    // 编辑题目：包含插入和更新题目（管理员权限）
    server.Post(API + "/admin/problem/edit", doEditProblem);
    // 删除题目（管理员权限）
    server.Delete(API + "/admin/problem/delete", doDeleteProblem);
    // 分页获取题目列表
    server.Get(API + "/problem/list", doGetProblemList);
    // 分页获取题目列表（管理员权限）
    server.Get(API + "/admin/problem/list", doGetProblemListByAdmin);
    // -------------------- 题目模块 End --------------------

    // --------------------  标签模块 Start --------------------
    // 获取题目的所有标签
    server.Get(API + "/tags", doGetTags);
    // --------------------  标签模块 End --------------------

    // -------------------- 公告模块 Start --------------------
    // 添加公告（管理员权限）
    server.Post(API + "/admin/announcement/insert", doInsertAnnouncement);
    // 查询公告详细信息，并将其浏览量加 1
    server.Get(API + "/announcement/info", doGetAnnouncement);
    // 查询公告的详细信息，主要是编辑时的查询
    server.Get(API + "/admin/announcement/info", doSelectAnnouncement);
    // 更新公告（管理员权限）
    server.Post(API + "/admin/announcement/update", doUpdateAnnouncement);
    // 删除公告（管理员权限）
    server.Delete(API + "/admin/announcement/delete", doDeleteAnnouncement);
    // 分页获取公告列表
    server.Get(API + "/announcement/list", doGetAnnouncementList);
    // 分页获取公告列表（管理员权限）
    server.Get(API + "/admin/announcement/list", doGetAnnouncementListByAdmin);
    // -------------------- 公告模块 End --------------------

    // --------------------  讨论模块 Start --------------------
    // 添加讨论
    server.Post(API + "/discussion/insert", doInsertDiscuss);
    // 查询讨论的详细内容，并且将其浏览量加 1
    server.Get(API + "/discussion/info", doGetDiscuss);
    // 查询讨论的详细信息，主要是编辑时的查询
    server.Get(API + "/discussion/select/info", doSelectDiscussByEdit);
    // 更新讨论
    server.Post(API + "/discussion/update", doUpdateDiscuss);
    // 删除讨论
    server.Delete(API + "/discussion/delete", doDeleteDiscuss);
    // 分页查询讨论
    server.Get(API + "/discussion/list", doGetDiscussList);
    // 分页查询讨论（管理员权限）
    server.Get(API + "/admin/discussion/list", doGetDiscussListByAdmin);
    // --------------------  讨论模块 End --------------------

    // -------------------- 题解模块 Start --------------------
    // 添加题解
    server.Post(API + "/solution/insert", doInsertSolution);
    // 查询题解的详细内容，并且将其浏览量加 1
    server.Get(API + "/solution/info", doGetSolution);
    // 查询题解的详细信息，主要是编辑时的查询
    server.Get(API + "/solution/select/info", doSelectSolutionByEdit);
    // 更新题解
    server.Post(API + "/solution/update", doUpdateSolution);
    // 删除题解
    server.Delete(API + "/solution/delete", doDeleteSolution);
    // 分页查询题解（公开题解）
    server.Get(API + "/solution/list", doGetSolutionList);
    // 分页查询题解（管理员权限）
    server.Get(API + "/admin/solution/list", doGetSolutionListByAdmin);
    // -------------------- 题解模块 End --------------------

    // -------------------- 评论模块 Start --------------------
    // 添加评论
    server.Post(API + "/comment/insert", doInsertComment);
    // 获取评论
    server.Get(API + "/comment/info", doGetComment);
    // 管理员查询评论
    server.Get(API + "/admin/comment/list", doGetCommentListByAdmin);
    // 删除评论
    server.Delete(API + "/comment/delete", doDeleteComment);
    // -------------------- 评论模块 End --------------------

    // -------------------- 测评记录模块 Start --------------------
    // 查询一条详细测评记录
    server.Get(API + "/status/record/info", doGetStatusRecord);
    // 返回状态记录的信息
    server.Get(API + "/status/record/list", doGetStatusRecordList);
    // -------------------- 测评记录模块 End --------------------

    // -------------------- 判题模块 Start --------------------
    // 返回判题信息
    server.Post(API + "/judge/code", doJudgeCode);
    // -------------------- 判题模块 End --------------------

    // -------------------- 图片模块 Start --------------------
    // 获取图片
    server.Get(API + R"(/image/(\d+))", doGetImage);
    // -------------------- 图片模块 End --------------------

    // 设置静态资源目录
    server.set_base_dir(constants::server::STATIC_ROOT);

    // 启动服务器，监听所有地址的指定端口
    cout << "========================================" << endl;
    cout << "HTTP Server starting ..." << endl;
    cout << "Host: " << constants::server::HOST << endl;
    cout << "Port: " << constants::server::PORT << endl;
    cout << "Thread Pool Size: " << constants::server::MAX_THREAD_COUNT << endl;
    cout << "Max Request Body: " << constants::server::MAX_REQUEST_BODY_SIZE / 1024 / 1024 << " MB" << endl;
    cout << "Request Timeout: " << constants::server::REQUEST_TIMEOUT_SECONDS << " s" << endl;
    cout << "========================================" << endl;

    if (!server.listen(constants::server::HOST, constants::server::PORT)) {
        cerr << "========================================" << endl;
        cerr << "HTTP Server failed to start!" << endl;
        cerr << "Please check if the port is already in use." << endl;
        cerr << "========================================" << endl;
    }
}

HttpServer::HttpServer() {
    // 构造函数实现
}

HttpServer::~HttpServer() {
    // 析构函数实现
}
