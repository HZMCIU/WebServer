#include "net/Buffer.h"

#include <assert.h>
#include <cstdio>
#include <fcntl.h>

#include <iostream>
#include <unistd.h>

using namespace std;

void testAppendAndRetrieve()
{
    Buffer buffer;
    size_t writable = buffer.writableBytes() / sizeof(int);
    for(int i = 0; i < writable; i++) {
        buffer.append((char*)&i, sizeof(i) / sizeof(char));
    }
    int *d = nullptr;
    for(int i = 0; i < writable; i++) {
        d = (int*)buffer.peek();
        assert(*d == i);
        buffer.retrieve(sizeof(int));
    }
}

void testMakeSpace()
{
    Buffer buffer;
    size_t writable = buffer.writableBytes() / sizeof(int);
    // fill whole buffer
    for(int i = 0; i < writable; i++) {
        buffer.append((char*)&i, sizeof(i) / sizeof(char));
    }
    // retrieve half buffer
    for(int i = 0; i < writable / 2; i++) {
        buffer.retrieve(sizeof(int) / sizeof(char));
    }
    for(int i = 0; i < writable; i++) {
        buffer.append((char*)&i, sizeof(i) / sizeof(char));
    }
    assert(buffer.readableBytes() == 1024 + 512);
    int *d = nullptr;
    for(int i = 0; i < writable / 2; i++) {
        d = (int*)buffer.peek();
        assert(*d == 128 + i);
        buffer.retrieve(sizeof(int) / sizeof(char));
    }

    for(int i = 0; i < writable; i++) {
        d = (int*)buffer.peek();
        assert(*d == i);
        buffer.retrieve(sizeof(int));
    }
}

void testReadFd()
{
    Buffer buffer;
    int fd = open("./input.txt", O_RDWR);
    int err = 0;
    buffer.readFd(fd, &err);
    string&& str=buffer.retrieveAsString(buffer.readableBytes());
    printf("Buffer::readFd %zu\n", str.length());
}
int main(int argc, char *argv[])
{
    testAppendAndRetrieve();
    testMakeSpace();
    testReadFd();
    return 0;
}
