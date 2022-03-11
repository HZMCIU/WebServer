#ifndef NET_EVENTLOOPTHREAD_H
#define NET_EVENTLOOPTHREAD_H

#include <functional>
#include <string>

#include "base/Condition.h"
#include "base/Mutex.h"
#include "base/noncopyable.h"
#include "base/Thread.h"

class EventLoop;
class EventLoopThread: noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThread(const ThreadInitCallback& cb, const std::string& name);
    ~EventLoopThread();
    EventLoop* startEventLoop();
private:
    EventLoop* loop_;
    void threadFunc();
    ThreadInitCallback callback_;
    Thread thread_;

    bool exiting_;
    std::string name_;

    MutexLock mutex_;
    Condition cond_;
};

#endif
