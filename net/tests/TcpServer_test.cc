#include "net/TcpServer.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include "net/SocketOps.h"
#include "net/EventLoop.h"
#include "net/Endian.h"
#include "net/TcpConnection.h"

using namespace std;

int main(int argc, char *argv[])
{
    EventLoop baseloop;
    TcpServer server(&baseloop, 3, 8080);
    server.start();
    baseloop.loop();
    return 0;
}
