#ifndef BASE_COUNTDOWNLATCH_H
#define BASE_COUNTDOWNLATCH_H

#include "base/Mutex.h"
#include "base/Condition.h"

class CountDownLatch {
public:
    explicit CountDownLatch (int count);
    void wait();
    void countDown();
    int getCount();
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};

#endif
