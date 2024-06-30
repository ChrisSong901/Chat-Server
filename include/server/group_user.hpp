#ifndef GROUP_USER_H
#define GROUP_USER_H
#include"user.hpp"
#include<string>
class GroupUser:public User{
public:
    GroupUser() = default;
    void set_role(const string & role)
    {
        _role = role;
    }
    string getrole()
    {
        return _role;
    }

private:
    string _role;
};

#endif