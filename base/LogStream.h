#ifndef BASE_LOGSTREAM_H
#define BASE_LOGSTREAM_H

#include <cstring>

#include <string>

#include "base/noncopyable.h"

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer: noncopyable {
public:
    FixedBuffer(): cur_(data_) {}
    ~FixedBuffer() {}
    int avail()
    {
        return end() - cur_;
    }
    int length () const
    {
        return static_cast<int>(cur_ - data_);
    }
    const char* data()const
    {
        return data_;
    }
    char* current()
    {
        return cur_;
    }
    void add(size_t len)
    {
        cur_ += len;
    }
    void append(const char* buf, size_t len)
    {
        if (static_cast<size_t>(avail()) > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }
    void reset()
    {
        cur_ = data_;
    }
    void bzero()
    {
        memset(data_, 0, sizeof data_);
    }
private:
    const char* end()
    {
        return data_ + sizeof(data_);
    }
    char data_[SIZE];
    char *cur_;
};


class LogStream: noncopyable {
public:
    using Buffer = FixedBuffer<kSmallBuffer>;
    LogStream& operator<<(bool v)
    {
        *this << (v == 0 ? "0" : "1");
        return *this;
    }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(float v)
    {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double);
    LogStream& operator<<(long double);


    LogStream& operator<<(char v)
    {
        buffer_.append(&v, 1);
        return *this;
    }
    LogStream& operator<<(const char* str)
    {
        if (str) {
            buffer_.append(str, strlen(str));
        }
        else {
            buffer_.append("(null)", 6);
        }
        return *this;
    }
    LogStream& operator<<(const unsigned char* str)
    {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(std::string& str)
    {
        buffer_.append(str.c_str(), str.size());
        return *this;
    }

    LogStream& operator<<(std::string&&str)
    {
        return operator<<(str);
    }
    void resetBuffer()
    {
        buffer_.reset();
    }

    const Buffer& buffer() const
    {
        return buffer_;
    }
private:
    template<typename T>
    void formatInteger(T t);
    Buffer buffer_;
    static const int kMaxNumericSize = 32;
};
#endif
