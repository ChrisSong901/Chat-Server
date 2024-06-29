#include"chatservice.hpp"
#include"public.hpp"
#include"muduo/base/Logging.h"
using namespace muduo;
Chatservice* Chatservice::instance()
{
    static Chatservice service;
    return &service;
}

//网络模块和业务模块解耦
Chatservice::Chatservice()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&Chatservice::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&Chatservice::reg, this, _1, _2, _3)});
}

void Chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    LOG_INFO << "do login service!!!";

}
void Chatservice::reg(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    LOG_INFO << "do reg service!!!";
}

MsgHandler Chatservice::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if(it==_msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgId: " << msgid << " can not find handler!";
        };
    }
    else{
        return _msgHandlerMap[msgid];
    }
    
}