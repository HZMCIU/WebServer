#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <cstdio>
#include <stddef.h>
#include <assert.h>

#include <vector>
#include <string>

#include "base/copyable.h"
class Buffer: copyable {
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
public:
    Buffer(size_t initialSize = kInitialSize)
        : buffer_(initialSize + kCheapPrepend)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
    {

    }
    size_t readableBytes()
    {
        return writerIndex_ - readerIndex_;
    }
    size_t writableBytes()
    {
        return buffer_.size() - writerIndex_;
    }
    size_t prependableBytes()
    {
        return readerIndex_;
    }

    void append(const char* data, size_t len)
    {
        ensureWrite(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }
    void prepend(const char* data, size_t len)
    {
        if (prependableBytes() >= len) {
            readerIndex_ -= len;
            std::copy(data, data + len, begin() + readerIndex_);
        }
    }
    void retrieve(size_t len)
    {
        if (len <= readableBytes()) {
            readerIndex_ += len;
        }
        else {
            retrieveAll();
        }
    }
    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }
    std::string retrieveAsString(size_t len)
    {
        if (len < readableBytes()) {
            readerIndex_ += len;
            return std::string(peek(), len);
        }
        else {
            return std::string(peek(), readableBytes());
        }
    }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }
    char* peek()
    {
        return begin() + readerIndex_;
    }
    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    void ensureWrite(size_t len)
    {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    ssize_t readFd(int fd, int* saveErrNo);
private:
    void makeSpace(size_t len)
    {
        if (prependableBytes() + writableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        }
        else {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }
    char* begin()
    {
        return &*buffer_.begin();
    }
    const char* begin() const
    {
        return &*buffer_.begin();
    }
    std::vector<char> buffer_;
    int readerIndex_;
    int writerIndex_;
};
#endif
