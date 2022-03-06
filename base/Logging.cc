#include "base/Logging.h"

#include <ctime>
#include <pthread.h>
#include <sys/time.h>

#include "base/AsyncLogging.h"

using namespace std;

static pthread_once_t once_control = PTHREAD_ONCE_INIT;

static AsyncLogging *asyncLogger = nullptr;

string Logger::logFileName_ = "./WebServer.log";

void once_init()
{
    asyncLogger = new AsyncLogging(Logger::getLogFileName());
    asyncLogger->start();
}

void output(const char* logline, size_t len)
{
    pthread_once(&once_control, once_init);
    asyncLogger->append(logline, len);
}

void Logger::Impl::formatTime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    time_t seconds = tv.tv_sec;
    struct tm* tm = nullptr;
    tm = localtime(&seconds);
    char buf[32];
    strftime(buf, sizeof buf, "%Y-%m-%d %H-%M-%S", tm);
    stream_ << buf;
}

Logger::Impl::Impl(const char* fileName, int line):
    stream_(),
    basename_(fileName),
    line_(line)
{
    formatTime();
}

Logger::Logger(const char* fileName, int line):
    impl_(fileName, line)
{
}

Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ":" << impl_.line_ << '\n';
    const LogStream::Buffer &buf(stream().buffer());
    output(buf.data(), buf.length());
}
