enable_testing()
add_executable(httpserver_test HttpServer_test.cc)
target_link_libraries(httpserver_test http)
add_test(NAME httpserver_test COMMAND httpserver_test)
