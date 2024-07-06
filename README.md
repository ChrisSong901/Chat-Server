## Chat-Server
在 Linux 环境下基于 muduo 开发的集群聊天服务器。实现新用户注册、用户登录、添加好友、添加群组、好友通信、群组聊天、保持离线消息等功能。

# 项目特点
基于muduo网络库开发网络核心模块，实现高效通信  
使用第三方JSON库实现通信数据的序列化和反序列化  
使用Nginx的TCP负载均衡功能，将客户端请求分派到多个服务器上，以提高并发处理能力  
基于发布-订阅的服务器中间件redis消息队列，解决跨服务器通信难题  
封装MySQL接口，将用户数据储存到磁盘中，实现数据持久化  
基于CMake构建项目  

# 项目环境
* boost库
* muduo库
* Nginx
* redis

# 构建项目

# 创建数据库
        连接MySQL
         mysql -u root -p your passward
        创建数据库
        create database chat;

# 执行脚本构建项目
        ./build.sh


# 执行可执行文件
        #服务端
        cd ./bin
        ./ChatServer 6000 

        #启动客户端
        ./ChatClient 127.0.0.1 8000

![success](https://github.com/ChrisSong901/Chat-Server/assets/172186531/11cc3c4f-6f59-43ec-92b8-a19a2a37e688)

## 项目讲解

# 创建数据库表
User表

        CREATE TABLE User (
            id INT PRIMARY KEY AUTO_INCREMENT,
            name VARCHAR(50) NOT NULL UNIQUE,
            password VARCHAR(50) NOT NULL,
            state ENUM('online', 'offline') DEFAULT 'offline'
        );
Friend表

        CREATE TABLE Friend (
            userid INT NOT NULL,
            friendid INT NOT NULL,
            PRIMARY KEY (userid, friendid)
        );

AllGroup表

        CREATE TABLE AllGroup (
            id INT PRIMARY KEY AUTO_INCREMENT,
            groupname VARCHAR(50) NOT NULL UNIQUE,
            groupdesc VARCHAR(200) DEFAULT ''
        );
GroupUser表

        CREATE TABLE GroupUser (
            groupid INT NOT NULL,
            userid INT NOT NULL,
            grouprole ENUM('creator', 'normal') DEFAULT 'normal',
            PRIMARY KEY (groupid, userid)
        );

OfflineMessage表

        CREATE TABLE OfflineMessage (
            userid INT NOT NULL,
            message VARCHAR(50) NOT NULL,
            PRIMARY KEY (userid)
        );

## 网络模块
Muduo采用了"one loop per thread + thread pool"的架构设计，即每个线程与一个事件循环(EventLoop)相对应，而这个事件循环则构成了一个Reactor模式的实例。在Muduo的上下文中，其Reactor模型被细化为主从Reactor模式，其中包含一个mainReactor和若干个subReactor。具体而言，mainReactor负责监听并接收新的连接请求，一旦有新连接到来，mainReactor会将其移交给subReactor进行后续的连接管理和数据传输处理。

![reactor](https://github.com/ChrisSong901/Chat-Server/assets/172186531/37dae97b-a2bf-4ad7-898f-93eb34a54224)

使用 muduo 注册消息事件到来的回调函数，并根据得到的 MSGID 定位到不同的处理函数中。以此实现业务模块和网络模块的解耦。

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
