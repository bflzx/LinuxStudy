#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <mutex>
#include <algorithm>
#include "user.hpp"
#include <mutex>
#include<algorithm>
using namespace std;

vector<User> m;
mutex mtx;

enum
{
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    ACCEPT_ERR,
};


void broadcastMessage(char* message,User& us)
{
    lock_guard<mutex> lg(mtx);
    for (int i = 0; i < m.size();i++)
    {
        if(m[i]._name != us._name)
        {
            string s = us._name +"#"+ message;
            size_t len = send(m[i]._sockfd, s.c_str(), s.size(), 0);
            if(len == -1)
            {
                cerr<<"向"<<m[i]._name<<"发送消息失败!"<<endl;
                m[i] = move(m[m.size() - 1]);
                m.pop_back();
                i--;
            }
            else
            {
                cout << "向用户" << m[i]._name << "发送消息成功!" << endl;
            }
        }
        else
        {
            cout << "已跳过用户:" << us._name << endl;
        }
    }
    cout << "广播结束!" << endl;
}

void* handleClient(void* sock)
{
    int client_sock = *((int *)sock);
    pthread_detach(pthread_self());
    //接收昵称
    char name[1024];
    recv(client_sock, name, sizeof(name), 0);
    User us(name, client_sock);
    string rc = "已创建成功!";
    unique_lock<mutex> ul(mtx);
    m.push_back(us);
    send(client_sock, rc.c_str(), rc.size(), 0);
    cout << "已登录用户数量:" << m.size() << endl;
    ul.unlock();

    //开始接收客户端消息
    char message[1024];
    while(true)
    {
        size_t r_len = recv(client_sock, message, sizeof(message), 0);
        if(r_len == 0)
        {
            cout << "用户" <<name << "断开连接!" << endl;
            ul.lock();
            auto pos = find(m.begin(), m.end(), us);
            m.erase(pos);
            cout << "已登录用户数量:" << m.size() << endl;
            ul.unlock();
            close(client_sock);
            break;
        }
        message[r_len] = 0;
        cout << "收到" << name << "的消息:"<<message<<"! 开始广播" << endl;
        broadcastMessage(message,us);
    }

    return nullptr;
}

class TcpServer
{
public:
    TcpServer(const uint16_t &port) : _listensock(-1), _port(port)
    {
    }
    void init()
    {
        // 创建socket套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock == -1)
        {
            cerr << "socket 套接字创建失败!" << endl;
            exit(SOCKET_ERR);
        }
        cout << "socket创建成功!" << endl;
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(_port);
        server_addr.sin_addr.s_addr = INADDR_ANY;
        // bind绑定端口
        if (bind(_listensock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            cerr << "bind端口失败!" << endl;
            exit(BIND_ERR);
        }
        cout << "bind端口成功!" << endl;

        // listen监听端口
        if (listen(_listensock, 10) == -1)
        {
            cerr << "listen 监听端口失败!" << endl;
            exit(LISTEN_ERR);
        }
        cout << "listen监听端口成功!" << endl;
    }

    void start()
    {
        while (true)
        {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int client_sock = accept(_listensock, (struct sockaddr *)&client_addr, &len);
            if (client_sock == -1)
            {
                cerr << "accept连接失败!" << endl;
                exit(ACCEPT_ERR);
            }
            
            cout << "\n新客户端连接: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << " (socket: " << client_sock << ")" << endl;
            int *sc = new int(client_sock);
            pthread_t pid;
            pthread_create(&pid, nullptr, handleClient, sc);
        }
    }

    ~TcpServer()
    {
    }

private:
    int _listensock;
    uint16_t _port;
};
