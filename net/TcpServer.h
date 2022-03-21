#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H

#include <cstdint>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>

#include "base/noncopyable.h"

class EventLoop;
class EventLoopThreadPool;
class Channel;
class TcpServer: noncopyable {
public:
    TcpServer(EventLoop* loop, int threadNum, uint16_t port = 80);
    void start();
private:
    void handleRead();
    EventLoop* baseloop_;
    std::shared_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    int threadNum_;

    int listeningFd_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;

    struct sockaddr_in listeningAddr_;
};
#endif
