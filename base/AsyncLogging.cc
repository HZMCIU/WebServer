#include "base/AsyncLogging.h"

#include <assert.h>

#include <exception>
#include <functional>

#include "base/Condition.h"
#include "base/CountDownLatch.h"
#include "base/Mutex.h"
#include "base/LogFile.h"

using namespace std;

AsyncLogging::AsyncLogging(string logFileName, const double flushInterval):
    running_(false),
    basename_(logFileName),
    flushInterval_(flushInterval),
    mutex_(),
    cond_(mutex_),
    thread_(bind(&AsyncLogging::threadFunc, this), "Logging"),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(),
    latch_(1)
{
    assert(logFileName.size() > 1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging()
{
    if (running_) {
        stop();
    }
}

void AsyncLogging::start()
{
    running_ = true;
    thread_.start();
    latch_.wait();
}

void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify();
    thread_.join();
}

void AsyncLogging::append(const char *logline, size_t len)
{
    MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len) {
        currentBuffer_->append(logline, len);
    }
    else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_) {
            currentBuffer_ = move(nextBuffer_);
        }
        else {
            currentBuffer_.reset(new Buffer);
        }

        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc()
{
    assert(running_ == true);
    latch_.countDown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();

    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while (running_) {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty()) {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();
            currentBuffer_ = move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for(size_t i = 0; buffersToWrite.size(); i++) {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2) {
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
