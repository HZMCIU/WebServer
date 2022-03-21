#include "net/TcpServer.h"

#include <cstdint>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/EventLoopThreadPool.h"
#include "net/SocketOps.h"
#include "net/Channel.h"
#include "net/Endian.h"
#include "base/Logging.h"


using namespace std;

TcpServer::TcpServer(EventLoop* loop, int threadNum, uint16_t port)
    : baseloop_(loop)
    , threadNum_(threadNum)
    , eventLoopThreadPool_(new EventLoopThreadPool(loop, "TcpServer"))
    , started_(false)
    , listeningFd_(sockets::createNonBlockingOrDie())
    , acceptChannel_(new Channel(loop, listeningFd_))
{
    eventLoopThreadPool_->setThreadNum(threadNum_);
    acceptChannel_->setReadCallback(bind(&TcpServer::handleRead, this));
    sockets::setIpPortDomain(&listeningAddr_, port);
    sockets::bindOrDie(listeningFd_, (struct sockaddr*)&listeningAddr_);
}

void TcpServer::start()
{
    eventLoopThreadPool_->start();
    sockets::listenOrDie(listeningFd_);
    acceptChannel_->enableReading();
    started_ = true;
}

void TcpServer::handleRead()
{
    int connfd = 0;
    struct sockaddr_in addr;
    memset(&addr, 0,  sizeof(addr));
    socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    if ((connfd = sockets::accept(listeningFd_, &addr)) == -1) {
        LOG << "TcpServer::handleRead";
        return ;
    }
    printf("peerAddr: %s\n", inet_ntoa(addr.sin_addr));
    char buf[] = "Hello World\n";
    write(connfd, buf, sizeof(buf));
    EventLoop* loop = eventLoopThreadPool_->getNextLoop();
    Channel* channel = new Channel(loop, connfd);
    channel->enableReading();
    channel->enableWriting();
    close(connfd);
}
