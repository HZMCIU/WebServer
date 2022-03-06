#ifndef BASE_CONDITION_H
#define BASE_CONDITION_H

#include <ctime>
#include <pthread.h>

#include "base/Mutex.h"

class Condition {
public:
    Condition(MutexLock& mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }
    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }
    void notify()
    {
        pthread_cond_signal(&pcond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }
    bool waitForSeconds(double seconds);

private:
    pthread_cond_t pcond_;
    MutexLock& mutex_;
};

#endif
