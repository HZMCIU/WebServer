#ifndef BASE_LOGFILE_H
#define BASE_LOGFILE_H

#include <string>
#include <memory>

#include "base/Mutex.h"
#include "base/FileUtil.h"
class LogFile {
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();
    void append(const char* logline, size_t len);
    void flush();
private:
    void append_unlocked(const char *logline, size_t len);
    std::string basename_;
    std::shared_ptr<MutexLock> mutex_;
    std::shared_ptr<AppendFile> file_;
    int flushEveryN_;
    int count_;
};

#endif
