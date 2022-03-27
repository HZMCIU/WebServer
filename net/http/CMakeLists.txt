set(http_SRCS
    HttpResponse.cc
    HttpServer.cc
    HttpContext.cc)
add_library(http ${http_SRCS})
target_link_libraries(http net)

add_subdirectory(tests)
