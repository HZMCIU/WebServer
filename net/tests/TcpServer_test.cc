#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/Endian.h"

using namespace std;

int main(int argc, char *argv[])
{
    EventLoop baseloop;
    TcpServer server(&baseloop, 3, 8080);
    server.start();
    baseloop.loop();
    return 0;
}
