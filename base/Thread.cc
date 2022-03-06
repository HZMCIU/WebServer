#include "base/Thread.h"

#include <assert.h>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/prctl.h>

#include "base/CountDownLatch.h"
#include "base/CurrentThread.h"

using namespace std;

namespace CurrentThread {
__thread pid_t t_cachedTid = 0;
__thread char t_tidString[32];
__thread int t_tidStringLength = 6;
__thread const char* t_threadName = "default";
}

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0) {
        t_cachedTid = gettid();
        t_tidStringLength =
            snprintf(t_tidString, sizeof t_tidStringLength, "%05d ", t_cachedTid);
    }
}

struct ThreadData {
    using ThreadFunc = Thread::ThreadFunc;
    ThreadFunc func_;
    string name_;
    pid_t* tid_;
    CountDownLatch* latch_;
    ThreadData(string& name, ThreadFunc &func, pid_t *tid, CountDownLatch *latch): name_(name), func_(func), tid_(tid), latch_(latch) {}
    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void* startThread(void *obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

void Thread::setDefaultName()
{
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name_ = buf;
    }
}
Thread::Thread(const ThreadFunc& func, string name):
    func_(func),
    name_(name),
    latch_(1),
    joined_(false),
    started_(false),
    tid_(0),
    pthreadId_(0)
{
    setDefaultName();
}

Thread::~Thread()
{
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData *data = new ThreadData(name_, func_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, NULL, startThread, data)) {
        started_ = false;
        delete data;
    }
    else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

void Thread::join()
{
    assert(started_);
    assert(!joined_);

    joined_=true;
    pthread_join(pthreadId_, NULL);
}
