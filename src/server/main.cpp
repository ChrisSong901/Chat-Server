#include<chatserver.hpp>
#include"chatservice.hpp"
#include"muduo/base/Logging.h"
#include<iostream>
#include<signal.h>
using namespace std;
void resetHandler(int)
{
    LOG_INFO << "capture the SIGINT, will reset state\n";
    Chatservice::instance()->reset();
    exit(0);
}
int main()
{

    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "chatserver");
    server.start();//启动服务
    loop.loop();//epoll_wait 阻塞等待
}