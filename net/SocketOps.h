#ifndef NET_SOCKETOPS_H
#define NET_SOCKETOPS_H

#include <cstdint>
struct sockaddr_in;
struct sockaddr;
namespace sockets {
int createNonBlockingOrDie();
void setNonBlockAndCloseOnExec(int sockfd);
int bindOrDie(int sockfd, struct sockaddr* addr );
int listenOrDie(int sockfd);
void setIpPortDomain(struct sockaddr_in * addr, uint16_t port);
int accept(int sockfd, struct sockaddr_in * addr);
}

#endif
