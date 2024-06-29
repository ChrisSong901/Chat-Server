#ifndef CHATSERVICE_H
#define CHATSERVICE_H


#include<unordered_map>
#include<muduo/net/TcpConnection.h>
#include<functional>

#include"json.hpp"

using namespace muduo;
using namespace muduo::net;

using namespace std;
using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

//聊天服务器业务类
class Chatservice{
public:
    //单例
    static Chatservice* instance();
    void login(const TcpConnectionPtr &conn, json &js, Timestamp);
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp);
    MsgHandler getHandler(int msgid);

private:
    //存储消息id和业务处理方法
    Chatservice();
    Chatservice(const Chatservice &) = delete;
    Chatservice(const Chatservice &&) = delete;
    Chatservice &operator=(const Chatservice &) = delete;

    unordered_map<int, MsgHandler> _msgHandlerMap;
};

#endif