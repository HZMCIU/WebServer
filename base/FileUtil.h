#ifndef BASE_FILEUTIL_H
#define BASE_FILEUTIL_H

#include <cstdio>

#include <string>

#include "base/noncopyable.h"

class AppendFile : noncopyable {
public:
    AppendFile(std::string filename);
    void append(const char* logline, size_t len);
    void flush();
    ~AppendFile();

private:
    FILE* fp_;
    size_t write(const char* logline, size_t len);
    char buffer_[64 * 1024];
};

#endif
