#include"friendmodel.hpp"
#include"db.h"
#include<vector>
using namespace std;
void FriendModel::insert(int userid, int friendid)
{
     // 组织sql语句
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into Friend values(%d, %d)", userid, friendid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};

    // 联合查询
    sprintf(sql, "select a.id, a.name, a.state from User a inner join Friend b on b.friendid = a.id where b.userid=%d", userid);
    vector<User> vec;
    MySQL mysql;
    if(mysql.connect())
    {
        MYSQL_RES* res= mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
        }
        return vec;
}