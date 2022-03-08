cmake_minimum_required(VERSION 3.0)
project(WebServer)

enable_testing()
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(base)
add_subdirectory(net)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

