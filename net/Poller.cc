#include "net/Poller.h"
#include "net/Channel.h"

Poller::Poller(EventLoop* loop): ownerLoop_(loop)
{

}

Poller::~Poller()=default;
