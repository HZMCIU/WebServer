#ifndef NET_HTTP_HTTPSERVER_H
#define NET_HTTP_HTTPSERVER_H

#include <cstdint>
#include <functional>
#include <string>

#include "base/noncopyable.h"
#include "net/Callbacks.h"
#include "net/EventLoop.h"
#include "net/TcpServer.h"

class HttpResponse;
class HttpRequest;
class HttpContext;

class HttpServer : noncopyable {
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse&)>;
    HttpServer(EventLoop* loop, std::string name = "HttpServer", uint16_t port = 80);
    void start();
    void setThreadNum(int num);
    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

private:
    void onMessage(const TcpConnectionPtr& ptr, Buffer* buf);
    void onConnection(const TcpConnectionPtr& ptr);
    void onRequest(const TcpConnectionPtr& ptr, const HttpRequest& req);
    TcpServer server_;
    int threadNum_;
    HttpCallback httpCallback_;
};
#endif
