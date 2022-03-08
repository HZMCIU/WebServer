#ifndef NET_POLLER_H
#define NET_POLLER_H

#include <map>
#include <vector>

#include "base/noncopyable.h"
#include "net/EventLoop.h"

class Channel;

class Poller: noncopyable {
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop *loop);
    /**
     * https://gcc.gnu.org/faq.html#vtables
     */
    virtual ~Poller() = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual int poll(ChannelList* channelList, int timeout) = 0;

    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;
    EventLoop *ownerLoop_;
};

#endif
