set(net_SRCS
    Channel.cc
    EventLoop.cc
    Poller.cc
    poller/DefaultPoller.cc
    poller/EPollPoller.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc
    SocketOps.cc
    TcpServer.cc
    TcpConnection.cc
    Buffer.cc)



add_subdirectory(tests)
add_subdirectory(http)
add_library(net ${net_SRCS})
target_link_libraries(net base)
