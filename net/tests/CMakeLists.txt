enable_testing()

add_executable(eventloop_test EventLoop_test.cc)
target_link_libraries(eventloop_test net)
add_test(NAME eventloop_test COMMAND eventloop_test)

add_executable(eventloopthreadpoll_test EventLoopThreadPool_test.cc)
target_link_libraries(eventloopthreadpoll_test net)
add_test(NAME eventloopthreadpoll_test COMMAND eventloopthreadpoll_test)

add_executable(tcpserver_test TcpServer_test.cc)
target_link_libraries(tcpserver_test net)
add_test(NAME tcpserver_test COMMAND tcpserver_test)

