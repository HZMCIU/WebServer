#include "net/Poller.h"
#include "net/poller/EPollPoller.h"

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    return new EPollPoller(loop);
}
