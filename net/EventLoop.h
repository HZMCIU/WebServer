#ifndef NET_EVENTLOOP_H
#define NET_EVENTLOOP_H

#include <atomic>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include "base/Mutex.h"
#include "base/noncopyable.h"

class Poller;
class Channel;

class EventLoop : noncopyable {
public:
    using Functor = std::function<void()>;
    using ChannelList = std::vector<Channel*>;

    EventLoop();
    ~EventLoop();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void loop();
    void quit();

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    bool isInLoopThread();
private:
    void doPendingFunctors();
    void wakeup();
    void handleRead();

    std::unique_ptr<Poller> poller_;
    int wakeupFd_;

    Channel* currentActiveChannel_;
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;

    bool eventHandling_;
    std::atomic<bool> quit_;
    bool looping_;
    pid_t pthreadId_;
    bool callingPendingFunctor_;
    uint64_t iteration_;

    std::vector<Functor> pendingFunctors_;
    mutable MutexLock mutex_;

    static const int kPollTimeout;
};
#endif
