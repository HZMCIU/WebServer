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
#include "net/SocketOps.h"

using namespace std;

using placeholders::_1;
using placeholders::_2;

void defaultHttpCallback(const HttpRequest& req, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponse::k404);
    resp.setStatusMessage("NOT FOUND");
    resp.setBody("<h1>404 NOT FOUND</h1>");
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
    char local[64], peer[64];
    sockets::toIpPort(&ptr->localAddress(), local, sizeof(local));
    sockets::toIpPort(&ptr->peerAddress(), peer, sizeof(peer));

    //cout << peer << " => "
    //     << local << " is " << (ptr->connected() ? "UP" : "DOWN") << endl;
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf)
{
    HttpContext* context = conn->getMutableContext();
    if (!context->parseHttpRequest(buf)) {
        conn->send("HTTP1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if (context->gotAll()) {
        HttpRequest request = context->getRequest();
        onRequest(conn, request);
        context->reset();
    }
    else {
        LOG << "HttpServer::onMessage";
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& ptr, const HttpRequest& req)
{
    HttpResponse resp;
    Buffer buf;
    httpCallback_(req, resp);
    resp.appendToBuffer(&buf);
    ptr->send((void*)buf.peek(), buf.readableBytes());
    ptr->shutdown();
}
