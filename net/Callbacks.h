#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include <memory>
#include <functional>

class TcpConnection;
class Buffer;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer* )>;

void defaultConnectionCallback(const TcpConnectionPtr&);
void defaultMessageCallback(const TcpConnectionPtr&, Buffer*);
#endif
