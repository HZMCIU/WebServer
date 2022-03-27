#include "net/SocketOps.h"

#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <unistd.h>

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


ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd, buf, count);
}

void sockets::toIpPort(struct sockaddr_in *addr, char *buf, size_t size)
{
    uint16_t port = networkToHost16(addr->sin_port);
    ::inet_ntop(AF_INET, &addr->sin_addr, buf, static_cast<socklen_t>(size));
    int len = strlen(buf);
    snprintf(buf + len, size - len, ":%u", port);
}

void sockets::shutdownWrite(int sockfd)
{
    ::shutdown(sockfd, SHUT_WR);
}

int sockets::close(int sockfd)
{
    int ret = 0;
    if ((ret =::close(sockfd)) < 0) {
        LOG << "sockets::close";
    }
    return ret;
}
