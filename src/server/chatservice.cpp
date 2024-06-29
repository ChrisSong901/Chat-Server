#include"chatservice.hpp"
#include"public.hpp"
#include"user.hpp"
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
        LOG_INFO << "getHandler SUCCESS";
        return _msgHandlerMap[msgid];
    }
    
}

void Chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    LOG_INFO << "do login service!!!";

}
void Chatservice::reg(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    LOG_INFO << "do regidster service!";
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setname(name);
    user.setpassword(pwd);
    bool state = usermodel.insert(user);
    if(state==true)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["error"] = 0;
        response["id"] = user.getid();
        conn->send(response.dump());
    }
    else{
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["error"] = 1;
        conn->send(response.dump());
    }

}
// void Chatservice::reg(const TcpConnectionPtr &conn, json s&j, Timestamp)
// {
//     LOG_DEBUG << "do regidster service!";
//     string name = js["name"];
//     string pwd = js["password"];
//     User user;
//     user.setname(name);
//     user.setpassword(pwd);
//     bool state = usermodel.insert(user);
//     if(state==true)
//     {
//         json response;
//         response["msgid"] = REG_MSG_ACK;
//         response["error"] = 0;
//         response["id"] = user.getid();
//         conn->send(response.dump());
//     }
//     else{
//         json response;
//         response["msgid"] = REG_MSG_ACK;
//         response["error"] = 1;
//         conn->send(response.dump());
//     }
// }
