#ifndef BASE_ASYNCLOGGING_H
#define BASE_ASYNCLOGGING_H

#include <memory>
#include <vector>

#include "base/Condition.h"
#include "base/CountDownLatch.h"
#include "base/LogStream.h"
#include "base/Mutex.h"
#include "base/Thread.h"
#include "base/noncopyable.h"

class AsyncLogging: noncopyable {
public:
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferPtr = std::shared_ptr<Buffer>;
    using BufferVector = std::vector<BufferPtr>;

    AsyncLogging(std::string basename, const double flushInterval=2);
    ~AsyncLogging();
    void append(const char* logline, size_t len);
    void start();
    void stop();
private:
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

    MutexLock mutex_;
    Condition cond_;
    CountDownLatch latch_;

    void threadFunc();
    Thread thread_;

    std::string basename_;
    bool running_;
    const double flushInterval_;
};
#endif
