#ifndef CHATSERVICE_H
#define CHATSERVICE_H


#include<unordered_map>
#include<muduo/net/TcpConnection.h>
#include<functional>
#include"usermodel.hpp"
#include"json.hpp"
#include<mutex>
#include"offlinemessagemodel.hpp"
#include"friendmodel.hpp"
#include"group.hpp"
#include"group_user.hpp"
#include"group_model.hpp"

#include"redis.hpp"
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
    void clientCloseExceptionHandler(const TcpConnectionPtr &conn);
    void OneChat(const TcpConnectionPtr &conn, json &js, Timestamp);

    void addFriendHandler(const TcpConnectionPtr &conn, json &js, Timestamp);
    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp);
     // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp);
    
    void redis_subscribe_message_handler(int channel, string message);

    // 服务端异常终止之后的操作
    void reset();

private:
    //存储消息id和业务处理方法
    Chatservice();
    Chatservice(const Chatservice &) = delete;
    Chatservice(const Chatservice &&) = delete;
    Chatservice &operator=(const Chatservice &) = delete;
    std::mutex _connMutex;
    unordered_map<int, MsgHandler> _msgHandlerMap;
    unordered_map<int, TcpConnectionPtr> _useConnMap;

    //操纵数据库的类
    UserModel usermodel;
    OfflineMessageModel offlineMsgModel;
    FriendModel friendmodel;
    GroupModel groupmodel;

    Redis _redis;

};

#endif