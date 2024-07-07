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

# 注册模块
我们从网络模块接收数据，根据 MSGID 定位到注册模块。从传递过来的 json 对象中获取用户 ID 和用户密码。并以此生成 User 对象，调用 model 层方法将新生成的 User 插入到数据库中。

# 登录模块
从 json 对象中获取用户ID和密码，并在数据库中查询获取用户信息是否匹配。如果用户已经登录过，即 state == "online"，则返回错误信息。登录成功后需要在改服务端的用户表中记录登录用户，并显示该用户的好友列表和收到的离线消息。

# 客户端异常退出模块
如果客户端异常退出了，我们会从服务端记录用户连接的表中找到该用户，如果它断连了就从此表中删除，并设置其状态为 offline。

# 服务端异常退出模块
如果服务端异常退出，它会将所有在线的客户的状态都设置为 offline。即，让所有用户都下线。异常退出一般是 CTRL + C 时，我们需要捕捉信号。这里使用了 Linux 的信号处理函数，我们向信号注册回调函数，然后在函数内将所有用户置为下线状态。

# 点对点聊天模块
通过传递的 json 查找对话用户 ID：

* 用户处于登录状态：直接向该用户发送信息
* 用户处于离线状态：需存储离线消息
# 添加好友模块
从 json 对象中获取添加登录用户 ID 和其想添加的好友的 ID，调用 model 层代码在 friend 表中插入好友信息。

# 群组模块
创建群组需要描述群组名称，群组的描述，然后调用 model 层方法在数据库中记录新群组信息。 加入群组需要给出用户 ID 和想要加入群组的 ID，其中会显示该用户是群组的普通成员还是创建者。 群组聊天给出群组 ID 和聊天信息，群内成员在线会直接接收到。

# 使用Nginx负载均衡模块
        负载均衡是什么？
假设一台机器支持两万的并发量，现在我们需要保证八万的并发量。首先想到的是升级服务器的配置，比如提高 CPU 执行频率，加大内存等提高机器的物理性能来解决此问题。但是单台机器的性能毕竟是有限的，而且也有着摩尔定律也日已失效。
这个时候我们就可以增加服务器的数量，将用户请求分发到不同的服务器上分担压力，这就是负载均衡。那我们就需要有一个第三方组件充当负载均衡器，由它负责将不同的请求分发到不同的服务器上。而本项目，我们选择 Nginx 的负载均衡功能。
![nginx](https://github.com/ChrisSong901/Chat-Server/assets/172186531/b0ea4a51-6952-49d1-a2f0-b9d4298f28bd)
# 选择 Nginx 的 tcp 负载均衡模块的原因：

* 把client的请求按照负载算法分发到具体的业务服务器ChatServer上
* 能够ChantServer保持心跳机制，检测ChatServer故障
* 能够发现新添加的ChatServer设备，方便扩展服务器数量
![nginx_conf](https://github.com/ChrisSong901/Chat-Server/assets/172186531/96514a19-26fc-4256-ad7a-7e1b8b5766de)

配置好后，重新加载配置文件启动。

        /usr/local/nginx/sbin/nginx -s reload

# redis发布-订阅功能解决跨服务器通信问题
我们之前的ChatServer是维护了一个连接的用户表，每次向别的用户发消息都会从用户表中查看对端用户是否在线。然后再判断是直接发送，还是转为离线消息。

但是现在我们是集群服务器，有多个服务器维护用户。我们的ChatServerA要聊天的对象在ChatServerB，ChatServerA在自己服务器的用户表中找不到。那么可能对端用户在线，它却给对端用户发送了离线消息。因此，我们需要保证跨服务器间的通信！那我们如何实现，非常直观的想法，我们可以让后端的服务器之间互相连接。

![p1](https://github.com/ChrisSong901/Chat-Server/assets/172186531/f5b8b7d3-8938-45d4-a16d-df14185b8be2)

上面的设计，让各个ChatServer服务器互相之间直接建立TCP连接进行通信，相当于在服务器网络之间进行广播。这样的设计使得各个服务器之间耦合度太高，不利于系统扩展，并且会占用系统大量的socket资源，各服务器之间的带宽压力很大，不能够节省资源给更多的客户端提供服务，因此绝对不是一个好的设计。

集群部署的服务器之间进行通信，最好的方式就是引入中间件消息队列，解耦各个服务器，使整个系统松耦合，提高服务器的响应能力，节省服务器的带宽资源

![p2](https://github.com/ChrisSong901/Chat-Server/assets/172186531/f6f1f0b6-778f-44bc-aa1e-5c65784fe568)

