enable_testing()

add_executable(eventloop_test EventLoop_test.cc)
target_link_libraries(eventloop_test net)
add_test(NAME eventloop_test COMMAND eventloop_test)
