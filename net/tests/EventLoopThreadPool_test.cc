#include "net/EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include <cstdlib>
#include <iostream>
#include <atomic>
#include <assert.h>

using namespace std;

int main(int argc, char *argv[])
{
    EventLoop baseloop;
    {
        EventLoopThreadPool model(&baseloop, "zero");
        model.setThreadNum(0);
        model.start();
        assert(&baseloop == model.getNextLoop());
    }

    {
        EventLoopThreadPool model(&baseloop, "one");
        model.setThreadNum(1);
        model.start();
        EventLoop* loop = model.getNextLoop();
        assert(&baseloop != loop);
        assert(loop == model.getNextLoop());
    }
    return 0;
}
