#ifndef __GROUP_H
#define __GROUP_H
#include<vector>
#include<string>
#include"group_user.hpp"

using namespace std;
class Group
{
public:
    Group(int id=-1,string name="",string desc="")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }
    void setId(int id)
    {
        this->id = id;
    }
    void setName(string name)
    {
        this->name = name;
    }
    void setDesc(string desc)
    {
        this->desc = desc;
    }
    int getId() { return id; }
    std::string getName() { return name; }
    std::string getDesc() { return desc; }
    std::vector<GroupUser> &getUsers() { return users;}
    

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;
};

#endif