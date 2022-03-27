#include "net/EventLoop.h"
#include "net/http/HttpServer.h"

int main(int argc, char *argv[])
{
    EventLoop loop;
    HttpServer server(&loop, "hxy", 8080);
    server.setThreadNum(3);
    server.start();
    loop.loop();
    return 0;
}
