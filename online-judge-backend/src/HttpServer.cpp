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

void doGetImage(const httplib::Request &req, httplib::Response &res)
{
    printf("doGetImage start!!!\n");
    int index = stoi(req.matches[1]);
    string path = "../WWW/image/avatar" + to_string(index) + ".webp";
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
    res.set_content(image, "webp");
}

void HttpServer::Run()
{
    using namespace httplib;
    Server server;

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