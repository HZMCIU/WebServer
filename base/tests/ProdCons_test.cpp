#include "base/CountDownLatch.h"
#include "base/CurrentThread.h"
#include "base/Mutex.h"
#include "base/Thread.h"
#include "base/LogStream.h"
#include "base/Condition.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define BUFFER_SIZE 10u
#define MAX 10
/**
 * @problem: dead lock
 */
void prodThread(Condition* empty, Condition* full, MutexLock* mutex, bool* empty_, bool* full_, size_t* size)
{
    int cnt = 0;
    timespec tm1, tm2;
    srand(time(0));
    tm1.tv_nsec = 1000000;
    while(cnt < MAX) {
        mutex->lock();
        while (*full_) {
            empty->wait();
        }
        printf("thread:%s produce %d left:%zu\n", CurrentThread::name(), ++cnt, *size);
        *size += 1;
        if (*size == BUFFER_SIZE) {
            *full_ = true;
        }
        *empty_ = false;
        full->notify();
        mutex->unlock();
        ::sleep(1);
    }
}

void consThread(Condition* empty, Condition* full, MutexLock* mutex, bool* empty_, bool* full_, size_t* size)
{
    int cnt = 0;
    timespec tm1, tm2;
    srand(time(0));
    tm1.tv_nsec = 10000000;
    while (cnt < MAX) {
        mutex->lock();
        while (*empty_) {
            full->wait();
        }
        printf("thread:%s consume %d left:%zu\n", CurrentThread::name(), ++cnt, *size);
        *size -= 1;
        if (*size == 0) {
            *empty_ = true;
        }
        *full_ = false;
        empty->notify();
        mutex->unlock();
        ::sleep(1);
    }
}

int main(int argc, char *argv[])
{
    printf("main: pid=%d,tid=%d\n", ::getpid(), CurrentThread::tid());
    MutexLock mutex, single;
    Condition empty(mutex), full(mutex);
    bool full_ = false;
    bool empty_ = true;
    size_t size = 0;
    Thread prod(bind(prodThread, &empty, &full, &mutex, &empty_, &full_, &size), "producer");
    Thread cons(bind(consThread, &empty, &full, &mutex, &empty_, &full_, &size), "consumer");
    prod.start();
    cons.start();
    empty.notify();
    prod.join();
    cons.join();
    return 0;
}
