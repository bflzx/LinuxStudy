#include<iostream>
#include<string>
#include<vector>
using namespace std;

class User
{
public:
    User(const string& name,int& sock):_name(name),_sockfd(sock)
    {}
    bool operator==(const User& other)const
    {
        return _name == other._name ? true : false;
    }
    ~User()
    {}
public:
    string _name;
    int _sockfd;
};