#include <cstdlib>
#include <iostream>

#include "constants/app.h"
#include "http/http_server.h"

using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << constants::app::APP_NAME << " v" << constants::app::VERSION << endl;

    HttpServer server;
    server.Run();
    return EXIT_SUCCESS;
}