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
int main(int argc,char** argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);
    signal(SIGINT, resetHandler);
    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "chatserver");
    server.start();//启动服务
    loop.loop();//epoll_wait 阻塞等待
}

/*
./ChatServer 127.0.0.1 6000

./ChatServer 127.0.0.1 6001

./ChatClient 127.0.0.1 8000


*/