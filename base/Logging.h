#ifndef BASE_LOGGING_H
#define BASE_LOGGING_H

#include <string>

#include "base/LogStream.h"

class Logger {
public:
    Logger(const char* fileName, int line);
    ~Logger();
    static const std::string& getLogFileName()
    {
        return logFileName_;
    }
    static void setLogFileName(const std::string& logFileName)
    {
        logFileName_ = logFileName;
    }
    LogStream& stream()
    {
        return impl_.stream_;
    }
private:
    class Impl {
    public:
        Impl(const char* fileName, int line);
        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };

    Impl impl_;
    static std::string logFileName_;
};

#define LOG Logger(__FILE__,__LINE__).stream()
#endif
