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
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&Chatservice::OneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&Chatservice::addFriendHandler, this, _1, _2, _3)});

    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&Chatservice::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&Chatservice::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&Chatservice::groupChat, this, _1, _2, _3)});
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
//{"msgid":1,"id":1,"password":"123456"}
void Chatservice::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    LOG_INFO << "do login service!!!";
    int id = js["id"].get<int>();
    string password = js["password"];
    User user = usermodel.query(id);
    if(user.getid()==id&&user.getpassword()==password)
    {
        if(user.getstate()=="online")
        {
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"]="this account is using, input another!";
            conn->send(response.dump());
        }
        else{
            {
                lock_guard<mutex> lock(_connMutex);
                _useConnMap.insert({id, conn});
            }

            user.setstate("online");
            usermodel.update(user);

            json response;
            response["id"] = user.getid();
            response["name"] = user.getname();
            response["state"] = user.getstate();

            vector<string> vec = offlineMsgModel.query(id);

            if(!vec.empty())
            {
                response["offlinemsg"] = vec;
                offlineMsgModel.remove(id);
            }
            else{
                LOG_INFO << "No Offline message!";
            }

            vector<User> userVec = friendmodel.query(id);
            if(!userVec.empty())
            {
                std::vector<string> vec;
                for(auto it:userVec)
                {
                    json js;
                    js["id"] = it.getid();
                    js["name"] = it.getname();
                    js["state"] = it.getstate();
                    vec.push_back(js.dump());
                }
                response["friend"] = vec;
            }

            conn->send(response.dump());
        }
    }else{
        LOG_INFO << "id false of password false!";
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 2;
        response["errmsg"]="id false of password false!";
        conn->send(response.dump());

    }
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
void Chatservice::clientCloseExceptionHandler(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _useConnMap.begin(); it != _useConnMap.end();it++)
        {
            if(it->second==conn)
            {
                user.setid(it->first);
                _useConnMap.erase(it);
                break;
            }
        }

    }
    if(user.getid()!=-1)
    {
        user.setstate("offline");
        usermodel.update(user);
    }
}
void Chatservice::OneChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int toid = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _useConnMap.find(toid);
        if(it!=_useConnMap.end())
        {
            it->second->send(js["msg"].dump());
            return;
        }
    }
    //不在线
    offlineMsgModel.insert(toid,js["msg"].dump());
}
void Chatservice::reset()
{
    usermodel.reset();
}
void Chatservice::addFriendHandler(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    friendmodel.insert(userid,friendid);
}
//创建群聊
void Chatservice::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();

    string name = js["groupname"];
    string desc = js["groupdesc"];
    Group group(-1, name, desc);
    if(groupmodel.createGroup(group))
    {
        groupmodel.addGroup(userid, group.getId(), "creator");
    }
}
    // 加入群组业务
void Chatservice::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    groupmodel.addGroup(userid, groupid,"normal");
}
// 群组聊天业务
void Chatservice::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{

    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> userIdVec = groupmodel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id:userIdVec)
    {
        auto it = _useConnMap.find(id);
        if(it!=_useConnMap.end())
        {
            it->second->send(js.dump());
        }
        else{
            offlineMsgModel.insert(id, js.dump());
        }
    }
}