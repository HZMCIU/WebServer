#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <functional>
#include <string>

#include "base/CountDownLatch.h"
#include "base/noncopyable.h"

class Thread: noncopyable {
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(const ThreadFunc& func, const std::string name=std::string());
    ~Thread();
    void start();
    void join();
    bool started()
    {
        return started_;
    }
    bool joined()
    {
        return joined_;
    }
    const std::string& name()const
    {
        return name_;
    }

private:
    void setDefaultName();
    ThreadFunc func_;
    bool started_;
    bool joined_;
    std::string name_;
    CountDownLatch latch_;
    pid_t tid_;
    pthread_t pthreadId_;
};

#endif
