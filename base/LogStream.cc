#include "base/LogStream.h"

#include <algorithm>
#include <cstdio>

using namespace std;

const char* digits = "9876543210123456789";
const char* zero = digits + 9;

template <typename T>
size_t convert(char buf[], T value)
{
    T tmp = value;
    char* p = buf;
    do {
        *p++ = zero[tmp % 10];
        tmp /= 10;
    }
    while (tmp != 0);
    if (value < 0) {
        *p++ = '-';
    }
    reverse(buf, p);
    return static_cast<size_t>(p - buf);
}

template <typename T>
void LogStream::formatInteger(T t)
{
    if (buffer_.avail() > kMaxNumericSize) {
        size_t len = convert(buffer_.current(), t);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    return operator<<(static_cast<int>(v));
}
LogStream& LogStream::operator<<(unsigned short v)
{
    return operator<<(static_cast<unsigned int>(v));
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}
LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v)
{
    if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
        buffer_.add(len);
    }
    return *this;
}
