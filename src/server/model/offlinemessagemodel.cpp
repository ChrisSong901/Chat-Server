#include"offlinemessagemodel.hpp"
#include"db.h"

void OfflineMessageModel::insert(int userId, std::string msg)
{
    // 组织sql语句
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into OfflineMessage values(%d, '%s')", userId, msg.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

void OfflineMessageModel::remove(int userid)
{
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "delete from OfflineMessage where userid = %d", userid);

    MySQL mysql;

    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<string> OfflineMessageModel::query(int userid)
{
     char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select message from OfflineMessage where userid = %d", userid);
    vector<string> vec;

    MySQL mysql;

    if(mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if(res!=nullptr)
        {
            MYSQL_ROW row;

            while((row=mysql_fetch_row(res))!=nullptr)
            {
                vec.emplace_back(row[0]);
            }
            mysql_free_result(res);

            return vec;
        }
    }

    return vec;
}
