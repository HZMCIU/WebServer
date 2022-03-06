#include "base/Condition.h"

#include <errno.h>

bool Condition::waitForSeconds(double seconds)
{
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    const long long kNanoSecondsPerSecond = 1000000000;
    long long nanoseconds = seconds * kNanoSecondsPerSecond;
    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
    abstime.tv_nsec += static_cast<long>(nanoseconds % kNanoSecondsPerSecond);
    return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
}
