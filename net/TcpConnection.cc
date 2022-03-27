#include "net/TcpConnection.h"

#include <cstdio>
#include <functional>

#include <iostream>

#include "base/Logging.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Callbacks.h"
#include "net/SocketOps.h"

using namespace std;

void defaultMessageCallback(const TcpConnectionPtr& ptr, Buffer* buffer)
{
    printf("%s\n", buffer->retrieveAsString(buffer->readableBytes()).c_str());
}

void defaultConnectionCallback(const TcpConnectionPtr& ptr)
{
    char local[30], peer[30];
    sockets::toIpPort(&ptr->localAddress(), local, sizeof local);
    sockets::toIpPort(&ptr->peerAddress(), peer, sizeof peer);

    LOG << peer << " => "
        << local << " is "
        << (ptr->connected() ? "UP" : "DOWN");
}
TcpConnection::TcpConnection(EventLoop* loop,
                             int sockfd,
                             const struct sockaddr_in& localAddr,
                             const struct sockaddr_in& peerAddr,
                             const string& name)
    : loop_(loop)
    , channel_(new Channel(loop, sockfd))
    , localAddr_(localAddr)
    , peerAddr_(peerAddr)
    , name_(name)
    , state_(kConnecting)
{
    channel_->setCloseCallback(bind(&TcpConnection::handleClose, this));
    channel_->setReadCallback(bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(bind(&TcpConnection::handleWrite, this));
}

TcpConnection::~TcpConnection()
{
    assert(state_ == kDisconnected);
    cout << "TcpConnection::dtor[" << name_ << "] at fd=" << channel_->fd() << endl;
    sockets::close(channel_->fd());
}
void TcpConnection::send(void* data, size_t len)
{
    if (loop_->isInLoopThread()) {
        sendInLoop(data, len);
    }
    else {
        loop_->runInLoop(bind(&TcpConnection::sendInLoop, this, data, len));
    }
}

void TcpConnection::sendInLoop(void* data, size_t len)
{
    int nwrite = 0;
    int remaining = len;
    nwrite = sockets::write(channel_->fd(), data, len);
    if (nwrite < 0) {
        LOG << "TcpConnection::sendInLoop";
        return ;
    }
    remaining -= nwrite;
    if (!channel_->isWriting() && remaining > 0) {
        outputBuffer_.append(static_cast<char*>(data) + nwrite, remaining);
        channel_->enableWriting();
    }
}

void TcpConnection::handleClose()
{
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guard(shared_from_this());

    connectionCallback_(guard);
    closeCallback_(guard);
}

void TcpConnection::handleWrite()
{
    if (channel_->isWriting()) {
        int nwrite = 0;
        nwrite = sockets::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if (nwrite < 0) {
            LOG << "TcpConnection::handlewrite";
        }
        outputBuffer_.retrieve(nwrite);
        if (outputBuffer_.readableBytes() == 0) {
            channel_->disableWriting();
            if (state_ == kDisconnecting) {
                shutdownInLoop();
            }
        }
    }
}

void TcpConnection::handleRead()
{
    int saveErrNo = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrNo);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_);
    }
    else if (n == 0) {
        handleClose();
    }
    else {
        LOG << "TcpConnection::handleRead";
    }
}

void TcpConnection::connectDestroyed()
{
    setState(kDisconnected);
    connectionCallback_(shared_from_this());
    channel_->remove();
}
void TcpConnection::connectEstablished()
{
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(bind(&TcpConnection::shutdownInLoop, this));
    }
}
void TcpConnection::shutdownInLoop()
{
    if (!channel_->isWriting()) {
        sockets::shutdownWrite(channel_->fd());
    }
}
