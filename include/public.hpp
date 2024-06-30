#ifndef PUBLIC_H
#define PUBLIC_H

enum EnMsgType
{
    LOGIN_MSG = 1,//登录
    LOGIN_MSG_ACK,
    LOGINOUT_MSG,
    REG_MSG,//注册
    REG_MSG_ACK,
    ONE_CHAT_MSG,
    ADD_FRIEND_MSG,

    CREATE_GROUP_MSG,//创建群聊
    ADD_GROUP_MSG,//添加群聊
    GROUP_CHAT_MSG,//群组聊天
};
/*
msgid
frmo

msg

to



*/
#endif