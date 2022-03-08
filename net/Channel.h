#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include <functional>

#include "base/noncopyable.h"

class EventLoop;

class Channel: noncopyable {
public:
    using EventCallback = std::function<void()>;
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();

    void setReadCallback(EventCallback readCallback)
    {
        readCallback_ = std::move(readCallback);
    }
    void setWriteCallback(EventCallback writeCallback)
    {
        writeCallback_ = std::move(writeCallback);
    }
    void setErrorCallback(EventCallback errorCallback)
    {
        errorCallback_ = std::move(errorCallback);
    }
    void setCloseCallback(EventCallback closeCallback)
    {
        closeCallback_ = std::move(closeCallback);
    }

    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    void disableReading()
    {
        events_ &= ~kReadEvent;
        update();
    }
    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        events_ = kNoneEvent;
        update();
    }
    void set_revents(int revt)
    {
        revents_ = revt;
    }
    int events()
    {
        return events_;
    }
    int fd()
    {
        return fd_;
    }
    int index()
    {
        return index_;
    }
    void set_index(int index)
    {
        index_ = index;
    }

    bool isNoneEvent()
    {
        return events_ == kNoneEvent;
    }
    void remove();

private:
    void update();

    int fd_;
    int index_;
    EventLoop* loop_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    bool addToLoop_;
    bool eventHandling_;

    int events_;
    int revents_;

    static const int kNoneEvent;
    static const int kWriteEvent;
    static const int kReadEvent;
};

#endif
