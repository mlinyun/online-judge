#include "http/http_server.h"

#include <httplib/httplib.h>  // 使用 httplib 作为 HTTP 服务器库
#include <json/json.h>        // 使用 JsonCpp 处理 JSON 数据

#include <fstream>  // C++17 文件系统库
#include <iostream>
#include <string>
#include <typeinfo>

#include "constants/server.h"
#include "core/control.h"

using namespace std;

Control control;  // 全局 Control 对象

// 根据返回结果设置状态码
bool SetResponseStatus(const Json::Value &json, httplib::Response &res) {
    string result = json["Result"].asString();
    if (result == "Success") {
        return true;
    } else if (result == "Fail") {
        return true;
    } else if (result == "400") {  // 请求参数有误
        res.status = 400;
    } else if (result == "401") {  // 无权限
        res.status = 401;
    } else if (result == "500") {  // 服务器出错啦
        res.status = 500;
    }
    return true;
}

// ------------------------------ 用户模块 Start ------------------------------
/**
 * 处理注册用户的请求
 */
void doRegisterUser(const httplib::Request &req, httplib::Response &res) {
    cout << "doRegister start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.RegisterUser(jsonvalue);
    cout << "doRegister end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理登录用户的请求
 */
void doLoginUser(const httplib::Request &req, httplib::Response &res) {
    cout << "doLoginUser start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.LoginUser(jsonvalue);
    cout << "doLoginUser end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取用户信息的请求
 */
void doGetUserInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserInfo start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        // 获取用户 ID 参数
        string userid = req.get_param_value("UserId");
        Json::Value queryjson;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserInfo(queryjson);
    }
    cout << "doGetUserInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理设置页面获取用户信息的请求
 */
void doGetUserUpdateInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserUpdateInfo start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string userid = req.get_param_value("UserId");
        Json::Value queryjson;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserUpdateInfo(queryjson);
    }
    cout << "doGetUserUpdateInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理更新用户信息的请求
 */
void doUpdateUserInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateUserInfo start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UpdateUserInfo(jsonvalue);
    cout << "doUpdateUserInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理删除用户的请求
 */
void doDeleteUser(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteUser start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("UserId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        // 获取用户 ID 参数
        string userid = req.get_param_value("UserId");
        Json::Value jsonvalue;
        jsonvalue["UserId"] = userid;
        resjson = control.DeleteUser(jsonvalue);
    }
    cout << "doDeleteUser end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取用户排名的请求
 */
void doGetUserRank(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserRank start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserRank(queryjson);
    }
    cout << "doGetUserRank end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取用户列表的请求（管理员权限）
 */
void doGetUserSetInfo(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetUserSetInfo start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserSetInfo(queryjson);
    }
    cout << "doGetUserSetInfo end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}
// ------------------------------ 用户模块 End ------------------------------

// ------------------------------ 题目模块 Start ------------------------------
/**
 * 处理获取单条题目信息的请求
 */
void doGetProblem(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblem start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("ProblemId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string problemid = req.get_param_value("ProblemId");
        Json::Value queryjson;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblem(queryjson);
    }
    cout << "doGetProblem end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取单条题目信息的请求（管理员权限）
 */
void doGetProblemInfoByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemInfoByAdmin start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("ProblemId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    } else {
        string problemid = req.get_param_value("ProblemId");

        Json::Value queryjson;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblemInfoByAdmin(queryjson);
    }
    cout << "doGetProblemInfoByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理编辑题目的请求：包含插入和更新题目（管理员权限）
 */
void doEditProblem(const httplib::Request &req, httplib::Response &res) {
    cout << "doEditProblem start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.EditProblem(jsonvalue["datainfo"]);
    cout << "doEditProblem end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理删除题目的请求（管理员权限）
 */
void doDeleteProblem(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteProblem start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("ProblemId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    } else {
        string problemid = req.get_param_value("ProblemId");
        Json::Value deletejson;
        deletejson["ProblemId"] = problemid;
        resjson = control.DeleteProblem(deletejson);
    }
    cout << "doDeleteProblem end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取题目列表的请求
 */
void doGetProblemList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemList start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("SearchInfo") || !req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    } else {
        Json::Value searchinfo;
        Json::Reader reader;
        // 解析传入的json
        reader.parse(req.get_param_value("SearchInfo"), searchinfo);
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["SearchInfo"] = searchinfo;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectProblemList(queryjson);
    }
    cout << "doGetProblemList end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取题目列表的请求（管理员权限）
 */
void doGetProblemListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetProblemListByAdmin start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectProblemListByAdmin(queryjson);
    }
    cout << "doGetProblemListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
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
    res.set_content(resjson.toStyledString(), "json");
}
// ------------------------------ 标签模块 End ------------------------------

// ------------------------------ 公告模块 Start ------------------------------
/**
 * 处理添加公告的请求（管理员权限）
 */
void doInsertAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertAnnouncement start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.InsertAnnouncement(jsonvalue);
    cout << "doInsertAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取单条公告信息的请求，并将其浏览量加 1
 */
void doGetAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncement start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("AnnouncementId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string announcementid = req.get_param_value("AnnouncementId");
        Json::Value queryjson;
        queryjson["AnnouncementId"] = announcementid;
        resjson = control.SelectAnnouncement(queryjson);
    }
    cout << "doGetAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理更新公告的请求（管理员权限）
 */
void doUpdateAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateAnnouncement start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UpdateAnnouncement(jsonvalue);
    cout << "doUpdateAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理删除公告的请求（管理员权限）
 */
void doDeleteAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteAnnouncement start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("AnnouncementId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string announcementid = req.get_param_value("AnnouncementId");
        Json::Value deletejson;
        deletejson["AnnouncementId"] = announcementid;
        resjson = control.DeleteAnnouncement(deletejson);
    }
    cout << "doDeleteAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取公告列表的请求
 */
void doGetAnnouncementList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncementList start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectAnnouncementList(queryjson);
    }
    cout << "doGetAnnouncementList end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取公告列表的请求（管理员权限）
 */
void doGetAnnouncementListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetAnnouncementListByAdmin start!!!" << endl;
    Json::Value resjson;
    // 请求参数校验
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectAnnouncementListByAdmin(queryjson);
    }
    cout << "doGetAnnouncementListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理查询公告的详细信息，主要是编辑时的查询
 */
void doSelectAnnouncement(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectAnnouncement start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("AnnouncementId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string announcementid = req.get_param_value("AnnouncementId");

        Json::Value queryjson;
        queryjson["AnnouncementId"] = announcementid;
        resjson = control.SelectAnnouncementByEdit(queryjson);
    }
    cout << "doSelectAnnouncement end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}
// ------------------------------ 公告模块 End ------------------------------

// ------------------------------ 讨论模块 Start ------------------------------
/**
 * 处理插入讨论的请求
 */
void doInsertDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertDiscuss start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.InsertDiscuss(jsonvalue);
    cout << "doInsertDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取单条讨论信息的请求
 */
void doGetDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscuss start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("DiscussId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string discussid = req.get_param_value("DiscussId");
        Json::Value queryjson;
        queryjson["DiscussId"] = discussid;
        resjson = control.SelectDiscuss(queryjson);
    }
    cout << "doGetDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理更新讨论的请求
 */
void doUpdateDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateDiscuss start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UpdateDiscuss(jsonvalue);
    cout << "doUpdateDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理删除讨论的请求
 */
void doDeleteDiscuss(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteDiscuss start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("DiscussId") || !req.has_param("UserId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string discussid = req.get_param_value("DiscussId");
        string userid = req.get_param_value("UserId");

        Json::Value deletejson;
        deletejson["DiscussId"] = discussid;
        deletejson["UserId"] = userid;
        resjson = control.DeleteDiscuss(deletejson);
    }
    cout << "doDeleteDiscuss end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取讨论列表的请求
 */
void doGetDiscussList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscussList start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("SearchInfo") || !req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        Json::Value searchinfo;
        Json::Reader reader;
        // 解析传入的json
        reader.parse(req.get_param_value("SearchInfo"), searchinfo);
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["SearchInfo"] = searchinfo;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectDiscussList(queryjson);
    }
    cout << "doGetDiscussList end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取讨论列表的请求（管理员权限）
 */
void doGetDiscussListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetDiscussListByAdmin start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectDiscussListByAdmin(queryjson);
    }
    cout << "doGetDiscussListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理查询讨论的详细信息，主要是编辑时的查询
 */
void doSelectDiscussByEdit(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectDiscussByEdit start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("DiscussId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string discussid = req.get_param_value("DiscussId");
        Json::Value queryjson;
        queryjson["DiscussId"] = discussid;
        resjson = control.SelectDiscussByEdit(queryjson);
    }
    cout << "doSelectDiscussByEdit end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}
// ------------------------------ 讨论模块 End ------------------------------

// ------------------------------ 题解模块 Start ------------------------------
/**
 * 处理插入题解的请求
 */
void doInsertSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doInsertSolution start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.InsertSolution(jsonvalue);
    cout << "doInsertSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理获取单条题解信息的请求
 */
void doGetSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolution start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("SolutionId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string solutionid = req.get_param_value("SolutionId");
        Json::Value queryjson;
        queryjson["SolutionId"] = solutionid;
        resjson = control.SelectSolution(queryjson);
    }
    cout << "doGetSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理更新题解的请求
 */
void doUpdateSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doUpdateSolution start!!!" << endl;
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UpdateSolution(jsonvalue);
    cout << "doUpdateSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理删除题解的请求
 */
void doDeleteSolution(const httplib::Request &req, httplib::Response &res) {
    cout << "doDeleteSolution start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("SolutionId") || !req.has_param("UserId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string solutionid = req.get_param_value("SolutionId");
        string userid = req.get_param_value("UserId");
        Json::Value deletejson;
        deletejson["SolutionId"] = solutionid;
        deletejson["UserId"] = userid;
        resjson = control.DeleteSolution(deletejson);
    }
    cout << "doDeleteSolution end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取题解列表的请求
 */
void doGetSolutionList(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolutionList start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("SearchInfo") || !req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        Json::Value searchinfo;
        Json::Reader reader;
        // 解析传入的json
        reader.parse(req.get_param_value("SearchInfo"), searchinfo);
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["SearchInfo"] = searchinfo;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectSolutionList(queryjson);
    }
    cout << "doGetSolutionList end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理分页获取题解列表的请求
 */
void doGetSolutionListByAdmin(const httplib::Request &req, httplib::Response &res) {
    cout << "doGetSolutionListByAdmin start!!!" << endl;
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectSolutionListByAdmin(queryjson);
    }
    cout << "doGetSolutionListByAdmin end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

/**
 * 处理查询题解的详细信息，主要是编辑时的查询
 */
void doSelectSolutionByEdit(const httplib::Request &req, httplib::Response &res) {
    cout << "doSelectSolutionByEdit start!!!" << endl;
    Json::Value resjson;

    if (!req.has_param("SolutionId")) {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    } else {
        string solutionid = req.get_param_value("SolutionId");
        Json::Value queryjson;
        queryjson["SolutionId"] = solutionid;
        resjson = control.SelectSolutionByEdit(queryjson);
    }
    cout << "doSelectSolutionByEdit end!!!" << endl;
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}
// ------------------------------ 题解模块 End ------------------------------

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

/**
 * 运行 HTTP 服务器
 */
void HttpServer::Run() {
    using namespace httplib;
    Server server;

    // 设置路由和处理函数
    // -------------------- 用户模块 Start --------------------
    // 注册用户
    server.Post("/api/user/register", doRegisterUser);
    // 登录用户
    server.Post("/api/user/login", doLoginUser);
    // 查询用户信息
    server.Get("/api/user/info", doGetUserInfo);
    // 查询用户信息（在设置页面修改用户时使用）
    server.Get("/api/user/updateinfo", doGetUserUpdateInfo);
    // 更新用户信息
    server.Post("/api/user/update", doUpdateUserInfo);
    // 删除用户
    server.Delete("/api/user/delete", doDeleteUser);
    // 用户排名查询
    server.Get("/api/user/rank", doGetUserRank);
    // 分页查询用户列表（管理员权限）
    server.Get("/api/user/list", doGetUserSetInfo);
    // -------------------- 用户模块 End --------------------

    // -------------------- 题目模块 Start --------------------
    // 查询题目信息（单条）
    server.Get("/api/problem/info", doGetProblem);
    // 查询题目信息（管理员权限）
    server.Get("/api/problem/admininfo", doGetProblemInfoByAdmin);
    // 编辑题目：包含插入和更新题目（管理员权限）
    server.Post("/api/problem/edit", doEditProblem);
    // 删除题目（管理员权限）
    server.Delete("/api/problem/delete", doDeleteProblem);
    // 分页获取题目列表
    server.Get("/api/problem/list", doGetProblemList);
    // 分页获取题目列表（管理员权限）
    server.Get("/api/problem/list/admin", doGetProblemListByAdmin);
    // -------------------- 题目模块 End --------------------

    // --------------------  标签模块 Start --------------------
    // 获取题目的所有标签
    server.Get("/tags", doGetTags);
    // --------------------  标签模块 End --------------------

    // -------------------- 公告模块 Start --------------------
    // 添加公告（管理员权限）
    server.Post("/api/announcement/insert", doInsertAnnouncement);
    // 查询公告详细信息，并将其浏览量加 1
    server.Get("/api/announcement/info", doGetAnnouncement);
    // 更新公告（管理员权限）
    server.Post("/api/announcement/update", doUpdateAnnouncement);
    // 删除公告（管理员权限）
    server.Delete("/api/announcement/delete", doDeleteAnnouncement);
    // 分页获取公告列表
    server.Get("/api/announcement/list", doGetAnnouncementList);
    // 分页获取公告列表（管理员权限）
    server.Get("/api/announcement/list/admin", doGetAnnouncementListByAdmin);
    // 查询公告的详细信息，主要是编辑时的查询
    server.Get("/api/announcement/admininfo", doSelectAnnouncement);

    // -------------------- 公告模块 End --------------------

    // --------------------  讨论模块 Start --------------------
    // 添加讨论
    server.Post("/api/discuss/insert", doInsertDiscuss);

    // 查询讨论的详细内容，并且将其浏览量加 1
    server.Get("/api/discuss/info", doGetDiscuss);

    // 更新讨论
    server.Post("/api/discuss/update", doUpdateDiscuss);

    // 删除讨论
    server.Delete("/api/discuss/delete", doDeleteDiscuss);

    // 分页查询讨论
    server.Get("/api/discuss/list", doGetDiscussList);

    // 分页查询讨论（管理员权限）
    server.Get("/api/discuss/list/admin", doGetDiscussListByAdmin);

    // 查询讨论的详细信息，主要是编辑时的查询
    server.Get("/api/discuss/admininfo", doSelectDiscussByEdit);
    // --------------------  讨论模块 End --------------------

    // -------------------- 题解模块 Start --------------------
    // 添加题解
    server.Post("/api/solution/insert", doInsertSolution);

    // 查询题解的详细内容，并且将其浏览量加 1
    server.Get("/api/solution/info", doGetSolution);

    // 更新题解
    server.Post("/api/solution/update", doUpdateSolution);

    // 删除题解
    server.Delete("/api/solution/delete", doDeleteSolution);

    // 分页查询题解（公开题解）
    server.Get("/api/solution/list", doGetSolutionList);

    // 分页查询题解（管理员权限）
    server.Get("/api/solution/list/admin", doGetSolutionListByAdmin);

    // 查询题解的详细信息，主要是编辑时的查询
    server.Get("/api/solution/admininfo", doSelectSolutionByEdit);
    // -------------------- 题解模块 End --------------------

    // 获取图片
    server.Get(R"(/api/image/(\d+))", doGetImage);

    // 设置静态资源目录
    server.set_base_dir(constants::server::STATIC_ROOT);

    // 启动服务器，监听所有地址的指定端口
    cout << "========================================" << endl;
    cout << "HTTP Server starting ..." << endl;
    cout << "Host: " << constants::server::HOST << endl;
    cout << "Port: " << constants::server::PORT << endl;
    cout << "========================================" << endl;

    server.listen(constants::server::HOST, constants::server::PORT);
}

HttpServer::HttpServer() {
    // 构造函数实现
}

HttpServer::~HttpServer() {
    // 析构函数实现
}
