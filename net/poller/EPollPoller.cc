#include "net/poller/EPollPoller.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <utility>
#include <cstring>

#include "net/Channel.h"
#include "net/Poller.h"

using namespace std;

namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

EPollPoller::EPollPoller(EventLoop* loop):
    Poller(loop),
    epollFd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(16)
{
}

EPollPoller::~EPollPoller()
{
    ::close(epollFd_);
}

int EPollPoller::poll(ChannelList *channelList, int timeout)
{
    int eventNum =::epoll_wait(epollFd_, &*events_.begin(), static_cast<int>(events_.size()), timeout);
    if (events_.size() == eventNum) {
        events_.resize(2 * events_.size());
    }
    for (int i = 0; i < eventNum; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        channelList->push_back(channel);
    }
    return eventNum;
}

void EPollPoller::updateChannel(Channel *channel)
{
    int index = channel->index();
    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            channels_[channel->fd()] = channel;
        }
        update(EPOLL_CTL_ADD, channel);
        channel->set_index(kAdded);
    }
    else {
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    ChannelMap::const_iterator it;
    if ((it = channels_.find(channel->fd())) != channels_.end()) {
        channels_.erase(it);
        update(EPOLL_CTL_DEL, channel);
    }
}

void EPollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    ::epoll_ctl(epollFd_, operation, fd, &event);
}
