#include "HttpServer.h"
#include "../include/httplib.h" // HTTP 库
#include <jsoncpp/json/json.h>  // JSON 库
#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include "Control.h"

using namespace std;
Control control;

// 根据返回结果设置状态码
bool SetResponseStatus(const Json::Value &json, httplib::Response &res)
{
    string result = json["Result"].asString();
    if (result == "Success")
    {
        return true;
    }
    else if (result == "Fail")
    {
        return true;
    }
    else if (result == "400") // 请求参数有误
    {
        res.status = 400;
    }
    else if (result == "401") // 无权限
    {
        res.status = 401;
    }
    else if (result == "500") // 服务器出错啦
    {
        res.status = 500;
    }
    return true;
}

// ++++++++++++++++++++++++++++++ 用户模块 Start ++++++++++++++++++++++++++++++
// 请求注册用户
void doRegisterUser(const httplib::Request &req, httplib::Response &res)
{
    printf("doRegister start!!!\n");
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.RegisterUser(jsonvalue);
    printf("doGetProblem end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求登录用户
void doLoginUser(const httplib::Request &req, httplib::Response &res)
{
    printf("doLoginUser start!!!\n");
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.LoginUser(jsonvalue);
    printf("doLoginUser end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求用户信息，用于主页展示
void doGetUserInfo(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetUserInfo start!!!\n");
    Json::Value resjson;
    if (!req.has_param("UserId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string userid = req.get_param_value("UserId");
        Json::Value queryjson;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserInfo(queryjson);
    }
    printf("doGetUserInfo end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求更新用户信息
void doUpdateUserInfo(const httplib::Request &req, httplib::Response &res)
{
    printf("doUpdateUserInfo start!!!\n");
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.UpdateUserInfo(jsonvalue);
    printf("doUpdateUserInfo end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求用户信息，用于编辑修改
void doGetUserUpdateInfo(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetUserUpdateInfo start!!!\n");
    Json::Value resjson;
    if (!req.has_param("UserId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string userid = req.get_param_value("UserId");
        Json::Value queryjson;
        queryjson["UserId"] = userid;
        resjson = control.SelectUserUpdateInfo(queryjson);
    }
    printf("doGetUserUpdateInfo end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 管理员请求分页获取用户信息
void doGetUserSetInfo(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetUserSetInfo start!!!\n");
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserSetInfo(queryjson);
    }
    printf("doGetUserSetInfo end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求删除用户
void doDeleteUser(const httplib::Request &req, httplib::Response &res)
{
    printf("doDeleteUser start!!!\n");
    Json::Value resjson;
    if (!req.has_param("UserId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string userid = req.get_param_value("UserId");

        Json::Value jsonvalue;
        jsonvalue["UserId"] = userid;
        resjson = control.DeleteUser(jsonvalue);
    }
    printf("doDeleteUser end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 请求用户排名
void doGetUserRank(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetUserRank start!!!\n");
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectUserRank(queryjson);
    }
    printf("doGetUserRank end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}
// ++++++++++++++++++++++++++++++ 用户模块 End ++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++ 题目模块 Start ++++++++++++++++++++++++++++++
// 返回网页请求的题目描述
void doGetProblem(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetProblem start!!!\n");
    Json::Value resjson;
    if (!req.has_param("ProblemId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "缺少请求参数！";
    }
    else
    {
        string problemid = req.get_param_value("ProblemId");
        Json::Value queryjson;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblem(queryjson);
    }
    printf("doGetProblem end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 返回题库
void doGetProblemList(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetProblemList start!!!\n");

    Json::Value resjson;
    if (!req.has_param("SearchInfo") || !req.has_param("Page") || !req.has_param("PageSize"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    }
    else
    {
        Json::Value serachinfo;
        Json::Reader reader;
        // 解析传入的json
        reader.parse(req.get_param_value("SearchInfo"), serachinfo);
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");

        Json::Value queryjson;
        queryjson["SearchInfo"] = serachinfo;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectProblemList(queryjson);
    }
    printf("doGetProblemList end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 返回管理员的题库
void doGetProblemListByAdmin(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetProblemListByAdmin start!!!\n");
    Json::Value resjson;
    if (!req.has_param("Page") || !req.has_param("PageSize"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    }
    else
    {
        string page = req.get_param_value("Page");
        string pagesize = req.get_param_value("PageSize");
        Json::Value queryjson;
        queryjson["Page"] = page;
        queryjson["PageSize"] = pagesize;
        resjson = control.SelectProblemListByAdmin(queryjson);
    }
    printf("doGetProblemListByAdmin end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 返回网页请求的题目描述
void doGetProblemInfo(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetProblemInfo start!!!\n");
    Json::Value resjson;

    if (!req.has_param("ProblemId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    }
    else
    {
        string problemid = req.get_param_value("ProblemId");

        Json::Value queryjson;
        queryjson["ProblemId"] = problemid;
        resjson = control.SelectProblemInfoByAdmin(queryjson);
    }
    printf("doGetProblemInfo end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 编辑题目 包含添加题目，修改题目(接口修改一下)
void doEditProblem(const httplib::Request &req, httplib::Response &res)
{
    printf("doEditProblem start!!!\n");
    Json::Value jsonvalue;
    Json::Reader reader;
    // 解析传入的json
    reader.parse(req.body, jsonvalue);
    Json::Value resjson = control.EditProblem(jsonvalue["datainfo"]);
    printf("doEditProblem end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 删除题目
void doDeleteProblem(const httplib::Request &req, httplib::Response &res)
{
    printf("doDeleteProblem start!!!\n");
    Json::Value resjson;
    if (!req.has_param("ProblemId"))
    {
        resjson["Result"] = "400";
        resjson["Reason"] = "请求参数有误！";
    }
    else
    {
        string problemid = req.get_param_value("ProblemId");

        Json::Value deletejson;
        deletejson["ProblemId"] = problemid;
        resjson = control.DeleteProblem(deletejson);
    }
    printf("doDeleteProblem end!!!\n");
    SetResponseStatus(resjson, res);
    res.set_content(resjson.toStyledString(), "json");
}

// 获取标签
void doGetTags(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetTags start!!!\n");
    Json::Value queryjson;
    string tagtype = req.get_param_value("TagType");
    queryjson["TagType"] = tagtype;

    Json::Value resjson = control.GetTags(queryjson);
    printf("doGetTags end!!!\n");
    res.set_content(resjson.toStyledString(), "json");
}
// ++++++++++++++++++++++++++++++ 题目模块 End ++++++++++++++++++++++++++++++

// 获取图片
void doGetImage(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetImage start!!!\n");
    int index = stoi(req.matches[1]);
    string path = "../WWW/image/avatar" + to_string(index) + ".png";
    ifstream infile;
    infile.open(path.data());
    if (!infile.is_open())
    {
        string str = "图片获取失败";
        res.set_content(str, "text");
    }
    string image((istreambuf_iterator<char>(infile)),
                 (istreambuf_iterator<char>()));
    printf("doGetImage end!!!\n");
    res.set_content(image, "png");
}

void HttpServer::Run()
{
    using namespace httplib;
    Server server;

    // ++++++++++++++++++++ 用户模块 Start ++++++++++++++++++++
    // 注册用户
    server.Post("/user/register", doRegisterUser);
    // 登录用户
    server.Post("/user/login", doLoginUser);
    // 返回用户信息，用于主页展示
    server.Get("/user/home", doGetUserInfo);
    // 更新用户信息
    server.Post("/user/update", doUpdateUserInfo);
    // 返回用户信息，用于编辑修改
    server.Get("/user/updateinfo", doGetUserUpdateInfo);
    // 分页获取用户信息
    server.Get("/userlist/admin", doGetUserSetInfo);
    // 删除用户
    server.Delete("/user", doDeleteUser);
    // 返回用户排名
    server.Get("/user/rank", doGetUserRank);
    // ++++++++++++++++++++ 用户模块 End ++++++++++++++++++++

    // ++++++++++++++++++++ 题目模块 Start ++++++++++++++++++++
    // 获取单个题目
    server.Get("/problem", doGetProblem);
    // 获取题库
    server.Get("/problemlist", doGetProblemList);
    // 管理员获取题库
    server.Get("/problemlist/admin", doGetProblemListByAdmin);
    // 获取单个题目详细信息
    server.Get("/problem/select", doGetProblemInfo);
    // 编辑题目 包含添加题目，修改题目(接口修改一下)
    server.Post("/problem/edit", doEditProblem);
    // 删除题目
    server.Delete("/problem", doDeleteProblem);

    // 获取标签
    server.Get("/tags", doGetTags);
    // ++++++++++++++++++++ 题目模块 End ++++++++++++++++++++

    // 获取图片资源
    server.Get(R"(/image/(\d+))", doGetImage);

    // 设置静态资源
    server.set_base_dir("../WWW");

    // 设置监听端口
    server.listen("0.0.0.0", 8081);
}

HttpServer::HttpServer()
{
}

HttpServer::~HttpServer()
{
}