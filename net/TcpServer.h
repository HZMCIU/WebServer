#ifndef NET_TCPSERVER_H
#define NET_TCPSERVER_H

#include <cstdint>
#include <netinet/in.h>
#include <sys/socket.h>

#include <map>
#include <memory>
#include <string>

#include "base/noncopyable.h"
#include "net/Callbacks.h"

class EventLoop;
class EventLoopThreadPool;
class Channel;
class TcpServer : noncopyable {
public:
    TcpServer(EventLoop* loop,
              uint16_t port = 80,
              const std::string& name = "TcpServer");
    ~TcpServer();
    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }
    void setThreadNum(int num);
private:
    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    void handleNewConnection();
    void startListening();
    EventLoop* baseloop_;
    std::shared_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    int threadNum_;

    int listeningFd_;
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;

    struct sockaddr_in listeningAddr_;
    ConnectionMap connections_;

    int nextConnId_;

    std::string name_;

    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;

    void removeConnection(const TcpConnectionPtr&);
    void removeConnectionInLoop(const TcpConnectionPtr&);
};
#endif
