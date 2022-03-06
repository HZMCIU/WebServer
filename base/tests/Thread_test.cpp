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
using Buffer = FixedBuffer<kLargeBuffer>;
using BufferPtr = shared_ptr<Buffer>;

void threadFunc()
{
    printf("thread: %s pid=%d tid=%d\n", CurrentThread::name(), ::getpid(), CurrentThread::tid());
}

/**
 * @problem: use reference for `CountDownLatch` will cause compiling error
 */
void bufferThread(const char* str, CountDownLatch* latch)
{
    latch->wait();
    BufferPtr newBuffer(new Buffer);
    newBuffer->append(str, strlen(str));
    printf("thread:%s pid=%d tid=%d content=%s\n", CurrentThread::name(), ::getpid(), CurrentThread::tid(), newBuffer->data());
}

int main(int argc, char *argv[])
{
    printf("main: pid=%d,tid=%d\n", ::getpid(), CurrentThread::tid());
    Thread basic(threadFunc,"basic thread");
    CountDownLatch latch(1);
    Thread bufThr(bind(bufferThread, "Hello World", &latch),"thread with latch and bind with parameter");
    basic.start();
    bufThr.start();

    latch.countDown();
    printf("bufThr start\n");
    basic.join();
    bufThr.join();

    return 0;
}
