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

/**
 * 处理获取图片的请求
 */
void doGetImage(const httplib::Request& req, httplib::Response& res) {
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
    } catch (const exception& e) {
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
