#include <cstdlib>

#include "http/http_server.h"

int main() {
    HttpServer server;
    server.Run();
    return EXIT_SUCCESS;
}