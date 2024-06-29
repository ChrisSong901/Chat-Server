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

