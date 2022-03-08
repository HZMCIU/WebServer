#ifndef NET_BASE_POLLER_EPOLLPOLLER_H
#define NET_BASE_POLLER_EPOLLPOLLER_H
#include "net/Poller.h"

struct epoll_event;
class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller() override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
    int poll(ChannelList* channelList, int timeout) override;

private:
    void update(int operation, Channel* channel);
    static const int kInitEventListSize = 16;
    int epollFd_;
    using EventList = std::vector<struct epoll_event>;
    EventList events_;
};
#endif
