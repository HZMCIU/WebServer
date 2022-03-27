#include "net/TcpServer.h"

#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "base/Logging.h"
#include "net/Callbacks.h"
#include "net/Channel.h"
#include "net/Endian.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/EventLoopThreadPool.h"
#include "net/SocketOps.h"
#include "net/TcpConnection.h"

using namespace std;

TcpServer::TcpServer(EventLoop* loop, uint16_t port, const string& name)
    : baseloop_(loop)
    , eventLoopThreadPool_(new EventLoopThreadPool(loop, "TcpServer"))
    , started_(false)
    , listeningFd_(sockets::createNonBlockingOrDie())
    , acceptChannel_(new Channel(loop, listeningFd_))
    , name_(name)
    , messageCallback_(defaultMessageCallback)
    , connectionCallback_(defaultConnectionCallback)
    , threadNum_(1)
{
    eventLoopThreadPool_->setThreadNum(threadNum_);
    acceptChannel_->setReadCallback(bind(&TcpServer::handleNewConnection, this));
    sockets::setIpPortDomain(&listeningAddr_, port);
    sockets::bindOrDie(listeningFd_, (struct sockaddr*)&listeningAddr_);
}

TcpServer::~TcpServer()
{
    for (auto& item : connections_) {
        TcpConnectionPtr conn(item.second);

        item.second.reset();
        conn->getLoop()->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
    }
}
void TcpServer::start()
{
    eventLoopThreadPool_->start();
    baseloop_->runInLoop(bind(&TcpServer::startListening, this));
    acceptChannel_->enableReading();
    started_ = true;
}

void TcpServer::setThreadNum(int num)
{
    threadNum_ = num;
}

void TcpServer::startListening()
{
    sockets::listenOrDie(listeningFd_);
}
void TcpServer::handleNewConnection()
{
    int connfd = 0;
    struct sockaddr_in peerAddr;
    EventLoop* ioLoop = eventLoopThreadPool_->getNextLoop();

    char buf[60];
    sockets::toIpPort(&listeningAddr_, buf, sizeof(buf));
    string connName = name_ + buf;
    snprintf(buf, sizeof buf, "-%s#%d", connName.c_str(), nextConnId_);
    nextConnId_++;

    memset(&peerAddr, 0, sizeof(peerAddr));
    socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    if ((connfd = sockets::accept(listeningFd_, &peerAddr)) == -1) {
        LOG << "TcpServer::handleRead";
        return;
    }

    TcpConnectionPtr conn(new TcpConnection(ioLoop, connfd, listeningAddr_, peerAddr, connName));

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(bind(&TcpServer::removeConnection, this, conn));
    ioLoop->runInLoop(bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(TcpConnectionPtr& conn)
{
    baseloop_->runInLoop(bind(&TcpServer::removeConnectionInLoop, this, conn));
}
void TcpServer::removeConnectionInLoop(TcpConnectionPtr& conn)
{
    EventLoop* ioLoop = conn->getLoop();
    int n = connections_.erase(conn->name());
    ioLoop->runInLoop(bind(&TcpConnection::connectDestroyed, conn));
}
