#ifndef USER_H
#define USER_H
#include <string>
using namespace std;
class User
{
public:
     User(int id = -1, std::string name = "", 
        std::string pwd = "", std::string state = "offline")
        : id(id),
          name(name),
          state(state)
    {}
    void setid(int id)
    {
        this->id = id;
    }
    void setname(string name)
    {
        this->name = name;
    }
    void setpassword(string password)
    {
        this->password = password;
    }
    void setstate(string state)
    {
        this->state = state;
    }
    int getid()
    {
        return this->id;
    }
    string getname()
    {
        return this->name;
    }
    string getpassword()
    {
        return this->password;
    }
    string getstate()
    {
        return this->state;
    }

protected : 
    int id;
    string name;
    string password;
    string state;
};

#endif