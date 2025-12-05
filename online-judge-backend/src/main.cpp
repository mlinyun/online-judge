#include <cstdlib>
#include <iostream>

#include "constants/app.h"
#include "http/http_server.h"
#include "utils/id_generator.hpp"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << constants::app::APP_NAME << " v" << constants::app::VERSION << endl;

    // 初始化 Snowflake（workerid=1，datacenterid=1）
    IDGenerator::Instance().Init(1, 1);

    HttpServer server;
    server.Run();
    return EXIT_SUCCESS;
}