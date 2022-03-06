#include "base/FileUtil.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string filename): fp_(fopen(filename.c_str(), "ae"))
{
    setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFile::~AppendFile()
{
    ::fclose(fp_);
}

void AppendFile::append(const char* logline, size_t len)
{
    size_t written = this->write(logline, len);
    size_t remain = len - written;
    while (remain > 0) {
        size_t n = this->write(logline + written, remain);
        if (n == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed.\n");
            }
            break;
        }
        written += n;
        remain -= n;
    }
}

void AppendFile::flush()
{
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
    return fwrite_unlocked(logline, sizeof(char), len, fp_);
}
