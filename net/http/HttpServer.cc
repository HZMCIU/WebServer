#include "net/http/HttpServer.h"

#include <cstdint>

#include <iostream>

#include "base/Logging.h"

#include "net/Callbacks.h"
#include "net/TcpServer.h"
#include "net/EventLoop.h"
#include "net/http/HttpContext.h"
#include "net/http/HttpRequest.h"
#include "net/http/HttpResponse.h"
#include "net/TcpConnection.h"

using namespace std;

using placeholders::_1;
using placeholders::_2;

void defaultHttpCallback(const HttpRequest& req, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponse::k404);
    resp.setStatusMessage("Not Found");
}

HttpServer::HttpServer(EventLoop* loop, string name, uint16_t port)
    : server_(loop, port, name)
    , httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(bind(&HttpServer::onMessage, this, _1, _2));
}

void HttpServer::setThreadNum(int num)
{
    server_.setThreadNum(num);
}

void HttpServer::start()
{
    server_.start();
    server_.setMessageCallback(bind(&HttpServer::onMessage, this, _1, _2));
}

void HttpServer::onConnection(const TcpConnectionPtr& ptr)
{
    cout << "Http Connection is established!" << endl;
}

void HttpServer::onMessage(const TcpConnectionPtr& ptr, Buffer* buf)
{
    HttpContext* context = ptr->getMutableContext();
    if (context->parseHttpRequest(buf)) {
        HttpRequest request = context->getRequest();
        HttpResponse response;
        onRequest(request, response);
        Buffer buf;
        response.appendToBuffer(&buf);
        // DEBUG
        printf("%s\n", buf.retrieveAsString(buf.readableBytes()).c_str());
        ptr->send((void*)buf.peek(), buf.readableBytes());
    }
    else {
        LOG << "HttpServer::onMessage";
    }
}

void HttpServer::onRequest(const HttpRequest& req, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponse::k200);
    resp.setStatusMessage("OK");
    string body = "<h1>Hello World</h1>";
    resp.setHeader("Content-Type", "text/html");
    resp.setHeader("Content-Length", to_string(body.size()));
    resp.setBody("<h1>Hello World</h1>");
}
