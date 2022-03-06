#include "base/LogFile.h"
#include "base/Mutex.h"

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN)
    : basename_(basename)
    , flushEveryN_(flushEveryN)
    , count_(0)
    , mutex_(new MutexLock())
{
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile(){}

void LogFile::flush()
{
    MutexLockGuard lock(*mutex_);
    file_->flush();
}
void LogFile::append(const char* logline, size_t len)
{
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::append_unlocked(const char* logline, size_t len)
{
    file_->append(logline, len);
    ++count_;
    if (count_ >= flushEveryN_) {
        file_->flush();
        count_ = 0;
    }
}
