#include"usermodel.hpp"
#include"db.h"
#include<iostream>
#include"muduo/base/Logging.h"
bool UserModel::insert(User & user)
{
    LOG_INFO << "UserModel::insert";
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into User(name, password, state) values('%s', '%s', '%s')",
        user.getname().c_str(), user.getpassword().c_str(), user.getstate().c_str());

    MySQL mysql;
    
    if(mysql.connect())
    {
        if (mysql.update(sql))
        {
            user.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }

        return true;
    }
    return false;
}
User UserModel::query(int id)
{
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select * from User where id = %d", id);
    
    MySQL mysql;
    
    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if(res!=nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row!=nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setpassword(row[2]);
                user.setstate(row[3]);
                mysql_free_result(res);
                return user;
            }

        }
    }
    return User();
}
bool UserModel::update(User user)
{
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "update User set state = '%s' where id =%d", user.getstate().c_str(), user.getid());
    
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}
void UserModel::reset()
{
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "update User set state = 'offline' where state = 'online'");
    
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}
