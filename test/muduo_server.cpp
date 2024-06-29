#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include<string.h>
#include"json.hpp"
#include <functional>
using namespace std;
using namespace placeholders;
using namespace muduo;
using namespace muduo::net;
class ChatServer
{

public:
    ChatServer(EventLoop *loop,
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

    void start()
    {
        _server.start();
    }

private:
    // 有新的连接，原用户的端开
    void onConnection(const TcpConnectionPtr &conn)
    {
        
        if(conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() <<"state: OnLine"<< endl;
        }else{
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() <<"state: OffLine"<< endl;
            conn->shutdown();
        }
    }
    //读写事件
    void onMessage(const TcpConnectionPtr &conn,//连接
                   Buffer *buffer,//缓冲区
                   Timestamp time)//时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        conn->send(buf);
    }
    TcpServer _server;

    EventLoop *_loop;
};
int main()
{
    using json = nlohmann::json;
    json js;
    js["id"] = {1, 2, 3, 4, 5};
    js["name"] = "zhang san";
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    cout << js << endl;

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "chatserver");
    server.start();//启动服务
    loop.loop();//epoll_wait 阻塞等待
}
/*
 update mysql.user set authentication_string=password('123456') where user='root' and host='localhost';

*/