#ifndef NET_SOCKETOPS_H
#define NET_SOCKETOPS_H

#include <cstdint>
#include <cstdio>
#include <stddef.h>
#include <sys/uio.h>

struct sockaddr_in;
struct sockaddr;
namespace sockets {
int createNonBlockingOrDie();
void setNonBlockAndCloseOnExec(int sockfd);
int bindOrDie(int sockfd, struct sockaddr* addr );
int listenOrDie(int sockfd);
void setIpPortDomain(struct sockaddr_in * addr, uint16_t port);
int accept(int sockfd, struct sockaddr_in * addr);
ssize_t read(int sockfd, void* buf, size_t count);
ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
int close(int sockfd);
}

#endif
