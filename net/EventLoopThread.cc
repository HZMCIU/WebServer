#include "net/EventLoopThread.h"

#include "base/Condition.h"
#include "base/Mutex.h"
#include "base/Thread.h"
#include "net/EventLoop.h"

using namespace std;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const string& name)
    : callback_(cb)
    , name_(name)
    , mutex_()
    , cond_(mutex_)
    , thread_(bind(&EventLoopThread::threadFunc, this), name)
    , loop_(nullptr)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_ != NULL) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startEventLoop()
{
    thread_.start();
    EventLoop* loop = nullptr;
    {
        MutexLockGuard lock(mutex_);
        while (loop_ == nullptr) {
            cond_.wait();
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    if (callback_) {
        callback_(&loop);
    }
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
    }
    cond_.notify();
    loop.loop();
}
