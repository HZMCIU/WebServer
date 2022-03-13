#include "net/EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"

using namespace std;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, const string& nameArg):
    baseloop_(baseloop),
    names_(nameArg),
    numThread_(0),
    started_(false),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::setThreadNum(int num)
{
    numThread_ = num;
}

void EventLoopThreadPool::start(ThreadInitCallback cb)
{
    for (int i = 0; i < numThread_; ++i) {
        char buf[names_.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", names_.c_str(), i);
        threads_.emplace_back(new EventLoopThread(cb, buf));
        loops_.emplace_back(threads_[i]->startEventLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseloop_;
    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if (next_ >= numThread_) {
            next_ = 0;
        }
    }
    return loop;
}

vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    vector<EventLoop*> loops;
    for(size_t i = 0; i < loops_.size(); i++) {
        loops.push_back(loops_[i]);
    }
    return loops;
}
