#include "net/SocketOps.h"

#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>

#include "base/Logging.h"
#include "net/Endian.h"

void sockets::setNonBlockAndCloseOnExec(int sockfd)
{
    int flags =::fcntl(sockfd, F_GETFD, 0);
    flags |= O_NONBLOCK;
    int ret =::fcntl(sockfd, F_SETFD, flags);

    flags =::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret =::fcntl(sockfd, F_SETFD, flags);
    (void) ret;
}

int sockets::createNonBlockingOrDie()
{
    int sockfd =::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        LOG << "sockets::createNonBlockingOrDie";
    }
    setNonBlockAndCloseOnExec(sockfd);
    return sockfd;
}

int sockets::bindOrDie(int sockfd, struct sockaddr *addr)
{
    int ret =::bind(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if (ret < 0) {
        LOG << "sockets::bindOrDie";
    }
    return ret;
}

int sockets::listenOrDie(int sockfd)
{
    int ret =::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG << "sockets::listenOrDie";
    }
    return ret;
}

void sockets::setIpPortDomain(struct sockaddr_in *addr, uint16_t port)
{
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = host16ToNetwork(port);
    addr->sin_family = AF_INET;
}

int sockets::accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t sock_len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int connfd = 0;
    if ((connfd =::accept(sockfd, static_cast<struct sockaddr*>((void*)addr), &sock_len)) == -1) {
        LOG << "sockets::accept" ;
        return -1;
    }
    setNonBlockAndCloseOnExec(connfd);
    return connfd;
}