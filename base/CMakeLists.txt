set(base_SRCS
    Condition.cc
    CountDownLatch.cc
    FileUtil.cc
    LogFile.cc
    LogStream.cc
    Thread.cc
    AsyncLogging.cc
    Logging.cc)


enable_testing()
add_library(base ${base_SRCS})
target_link_libraries(base pthread rt)
add_subdirectory(tests)

