#pragma once

#include <iostream>
#include <string>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<cerrno>
#include<cstring>
#include<cstdlib>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<functional>

namespace Server
{
    using namespace std;
    const static string defaultIp = "0.0.0.0";
    static const int gnum = 1024;
    enum
    {
        USAGE_ERR = 1,
        SOCKET_ERR = 2,
        BIND_ERR,
        OPEN_ERR
    };

    using func_t = function<void(int,string, uint16_t, string)>;

    class udpServer
    {
    public:
        udpServer(func_t cl,const uint16_t& port,const string& ip = defaultIp)
        :_callback(cl),_port(port),_ip(ip),_sockfd(-1)
        {}
        void initServer()
        {
            //1.创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if(_sockfd == -1)
            {
                cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
                exit(SOCKET_ERR);
            }
            cout << "socket success:" << ":" << _sockfd << endl;
            // 2.绑定ip,port
            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            //local.sin_addr.s_addr = inet_addr(_ip.c_str());
            local.sin_addr.s_addr = INADDR_ANY;//任意地址bind

            int n = bind(_sockfd, (struct sockaddr*)&local,sizeof(local));
            if(n == -1)
            {
                cerr << "bind error:" << errno << ":" << strerror(errno) << endl;
                exit(BIND_ERR);
            }
        }
        void start()
        {
            char buffer[gnum];
            for (;;)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
                if (s > 0)
                {
                    buffer[s] = 0;
                    string clientip = inet_ntoa(peer.sin_addr);
                    uint16_t clientport = ntohs(peer.sin_port);
                    string message = buffer;

                    cout << clientip << "[" << clientport << "]#" << message << endl;

                    _callback(_sockfd,clientip,clientport,message);
                }
            }
        }
        ~udpServer()
        {
        }

    private:
        uint16_t _port;
        string _ip;
        int _sockfd;
        func_t _callback;
    };
}
