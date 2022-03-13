set(net_SRCS
    Channel.cc
    EventLoop.cc
    Poller.cc
    poller/DefaultPoller.cc
    poller/EPollPoller.cc
    EventLoopThread.cc
    EventLoopThreadPool.cc)


add_subdirectory(tests)
add_library(net ${net_SRCS})
target_link_libraries(net base)

