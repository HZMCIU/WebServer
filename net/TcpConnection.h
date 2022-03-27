#ifndef NET_TCPCONNECTION_H
#define NET_TCPCONNECTION_H
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#include <memory>

#include "base/noncopyable.h"
#include "net/EventLoop.h"
#include "net/Callbacks.h"
#include "net/Buffer.h"
#include "net/http/HttpContext.h"

class TcpConnection: noncopyable,
    public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop,
                  int sockfd,
                  const struct sockaddr_in& localAddr,
                  const struct sockaddr_in& peerAddr,
                  const std::string& name);
    ~TcpConnection();

    EventLoop* getLoop()
    {
        return loop_;
    }
    
    const std::string& name()
    {
        return name_;
    }

    void send(void *data, size_t len);
    void shutdown();

    struct sockaddr_in& localAddress()
    {
        return localAddr_;
    }
    struct sockaddr_in& peerAddress()
    {
        return peerAddr_;
    }

    void setConnectionCallback(const ConnectionCallback& cb)
    {
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback& cb)
    {
        messageCallback_ = cb;
    }
    void setCloseCallback(const CloseCallback& cb)
    {
        closeCallback_ = cb;
    }

    void connectEstablished();
    void connectDestroyed();

    bool connected()
    {
        return state_ == kConnected;
    }

    HttpContext* getMutableContext()
    {
        return  &context_;
    }
    Buffer* outputBuffer()
    {
        return &outputBuffer_;
    }
    Buffer* inputBuffer()
    {
        return &inputBuffer_;
    }
private:
    enum StateE {kDisconnected, kConnecting, kConnected, kDisconnecting};

    void setState(StateE s)
    {
        state_ = s;
    }

    void sendInLoop(void* data, size_t len);
    void shutdownInLoop();

    void handleRead();
    void handleWrite();
    void handleClose();


    EventLoop* loop_;
    std::string name_;
    struct sockaddr_in localAddr_;
    struct sockaddr_in peerAddr_;
    std::shared_ptr<Channel> channel_;

    StateE state_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    HttpContext context_;
};
#endif
