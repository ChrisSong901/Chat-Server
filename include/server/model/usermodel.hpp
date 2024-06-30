#ifndef USERMODEL_H
#define USERMODEL_H
#include"user.hpp"

class UserModel{
public:
    bool insert(User &user);
    User query(int id);
    bool update(User user);
    void reset();
};

#endif