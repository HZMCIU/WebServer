#include "net/EventLoop.h"

#include <cstdint>
#include <unistd.h>
#include <sys/eventfd.h>

#include <memory>

#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Mutex.h"

#include "net/Poller.h"
#include "net/Channel.h"

using namespace std;

int createEventFd()
{
    int fd =::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG << "failed in eventfd";
        abort();
    }
    return fd;
}

const int EventLoop::kPollTimeout = 10000;

EventLoop::EventLoop():
    quit_(false),
    looping_(false),
    eventHandling_(false),
    iteration_(0),
    callingPendingFunctor_(false),
    currentActiveChannel_(nullptr),
    pthreadId_(CurrentThread::tid()),
    wakeupFd_(createEventFd()),
    poller_(Poller::newDefaultPoller(this)),
    wakeupChannel_(new Channel(this, wakeupFd_))
{
    wakeupChannel_->setReadCallback(bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::quit()
{
    MutexLockGuard lock(mutex_);
    quit_ = true;
    looping_ = false;
    wakeup();
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(&activeChannels_, kPollTimeout);

        ++iteration_;
        eventHandling_ = true;
        for(Channel* channel : activeChannels_) {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent();
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
        doPendingFunctors();
    }
}

void EventLoop::runInThread(Functor cb)
{
    if (isInLoopThread()) {
        cb();
    }
    else {
        queueInThread(cb);
    }
}

void EventLoop::queueInThread(Functor cb)
{
    MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(move(cb));
    wakeup();
}

void EventLoop::doPendingFunctors()
{
    vector<Functor> functors;
    callingPendingFunctor_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(const Functor& cb : pendingFunctors_) {
        cb();
    }
    callingPendingFunctor_ = false;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int n = write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG << "EventLoop::wakeup() write " << n << "bytes instead of 8.";
    }
}

void EventLoop::handleRead()
{
    uint64_t one;
    int n =::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG << "EventLoop::handleRead() read " << n << "bytes instead of 8.";
    }
}

bool EventLoop::isInLoopThread()
{
    return pthreadId_ == CurrentThread::tid();
}
