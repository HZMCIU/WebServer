#include "base/LogFile.h"

#include <cstring>
using namespace std;

int main(int argc, char *argv[])
{
    LogFile logFile("./WebServer.log");
    const char *logline = "This is a successful log line";
    logFile.append(logline, strlen(logline));
    logFile.flush();
    return 0;
}
