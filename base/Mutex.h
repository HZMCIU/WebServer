#ifndef BASE_MUTEX_H
#define BASE_MUTEX_H

#include <pthread.h>

#include "base/noncopyable.h"

class MutexLock : noncopyable {
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~MutexLock()
    {
        pthread_mutex_lock(&mutex_);
        pthread_mutex_destroy(&mutex_);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t * getPthreadMutex()
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
private:
    friend class Condition;
};


class MutexLockGuard{
public:
    MutexLockGuard(MutexLock &mutex): mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
};

#endif
