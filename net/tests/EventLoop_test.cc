#include "base/CurrentThread.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    printf("main(): pid=%d tid=%d\n", ::getpid(), CurrentThread::tid());
    EventLoop* loop = nullptr;
    {
        EventLoopThread thread_(nullptr, "eventLoopThread");
        loop = thread_.startEventLoop();
        loop->runInThread([]() {
            cout << "Hello World" << endl;
        });
        ::sleep(3);
    }
    return 0;
}
