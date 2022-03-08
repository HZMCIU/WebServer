#include "base/CurrentThread.h"
#include "net/EventLoop.h"
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
    printf("main(): pid=%d tid=%d\n", ::getpid(), CurrentThread::tid());
    EventLoop loop;
    loop.loop();
    ::sleep(1);
    loop.quit();
    return 0;
}
