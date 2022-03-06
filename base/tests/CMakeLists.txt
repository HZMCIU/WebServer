add_executable(logging_test Logging_test.cpp)
target_link_libraries(logging_test base)
add_test(NAME logging_test COMMAND logging_test)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test base)
add_test(NAME thread_test COMMAND thread_test)

add_executable(logfile_test LogFile_test.cpp)
target_link_libraries(logfile_test base)
add_test(NAME logfile_test COMMAND logfile_test)

add_executable(logstream_test LogStream_test.cpp)
target_link_libraries(logstream_test base)
add_test(NAME logstream_test COMMAND logstream_test)

add_executable(prodcons_test ProdCons_test.cpp)
target_link_libraries(prodcons_test base)
add_test(NAME prodcons_test COMMAND prodcons_test)
