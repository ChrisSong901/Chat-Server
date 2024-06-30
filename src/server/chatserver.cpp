#include "chatserver.hpp"
#include "json.hpp"
#include"chatservice.hpp"
#include <iostream>
#include <functional>
#include <string>
#include"muduo/base/Logging.h"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 给服务器注册用户连接和断开的回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    // 给服务器注册用户读写的回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        Chatservice::instance()->clientCloseExceptionHandler(conn);
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, // 连接
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)               // 时间信息
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);
    auto msgHandler= Chatservice::instance()->getHandler(js["msgid"].get<int>());
    LOG_INFO << "begining!";
    msgHandler(conn, js, time);
}