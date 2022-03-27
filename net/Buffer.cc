#include "net/Buffer.h"

#include <cstdio>
#include <errno.h>

#include "net/SocketOps.h"

const char Buffer::kCRLF[]="\r\n";

ssize_t Buffer::readFd(int fd, int *saveErrNo)
{
    char extrabuf[65536];
    struct iovec vec[2];
    size_t writable = writableBytes();

    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    int cnt = writable < sizeof(extrabuf) ? 2 : 1;
    ssize_t n = sockets::readv(fd, vec, cnt);
    if (n < 0) {
        *saveErrNo = errno;
    }
    else if (n <= writable) {
        writerIndex_ += n;
    }
    else {
        writerIndex_ = buffer_.size();
        ensureWrite(n - writable);
        append(extrabuf, n - writable);
    }
    return n;
}
