#include "net/Channel.h"
#include "base/Logging.h"
#include "net/EventLoop.h"

#include <poll.h>
#include <sys/poll.h>

using namespace std;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : fd_(fd)
    , loop_(loop)
    , events_(0)
    , revents_(0)
    , eventHandling_(false)
    , addToLoop_(false)
    , index_(-1)
{
}

void Channel::update()
{
    addToLoop_ = true;
    loop_->updateChannel(this);
}

Channel::~Channel()
{
}

void Channel::handleEvent()
{
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        LOG << "fd = " << fd_ << " Channel::handleEvent() POLLHUP";
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & POLLNVAL) {
        LOG << "fd = " << fd_ << " Channel::handleEvent() POLLNVAL";
    }

    if (revents_ & (POLLERR | POLLHUP)) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) {
            readCallback_();
        }
    }

    if (revents_ & (POLLOUT)) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}

void Channel::remove()
{
    addToLoop_ = false;
    loop_->removeChannel(this);
}
