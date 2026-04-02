#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <functional>
#include"Protocol.hpp"

namespace server
{
    using namespace std;
    enum
    {
        USAGE_ERR = 1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR,

    };
    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    using func_t = function<bool(const HttpRequest &, HttpResponse &)>;

    class HttpServer
    {
    public:
        HttpServer(func_t func,const uint16_t &port = 8080)
            : _func(func),_listensock(-1), _port(port)
        {
        }
        void initServer()
        {
            // 1.创建socket套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock < 0)
            {
                exit(SOCKET_ERR);
            }

            // 2.bind
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                exit(BIND_ERR);
            }

            // 设置监听状态
            if (listen(_listensock, gbacklog) < 0)
            {
                exit(LISTEN_ERR);
            }
        }
        void HandlerHttp(int sock)
        {
            //1.读到完整的http请求
            //2.反序列化
            //3.httprequest,httpresponse,_func(req,resp)
            //4.resp序列化
            //5.send

            char buffer[4096];
            HttpRequest req;
            HttpResponse resp;
            size_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if(n >0)
            {
                buffer[n] = 0;
                req.inbuffer = buffer;
                req.parse();
                _func(req, resp);
                send(sock, resp.outbuffer.c_str(),resp.outbuffer.size(),0);

            }
        }
        void start()
        {
            for (;;)
            {
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listensock, (struct sockaddr *)&peer, &len);
                if (sock < 0)
                {
                    continue;
                }

                pid_t id = fork();
                if (id == 0) // child
                {
                    close(_listensock);
                    if(fork()>0)
                    {
                        exit(0);
                    }
                    HandlerHttp(sock);
                    close(sock);
                    exit(0);
                }
                close(sock);
                waitpid(id, nullptr, 0);
            }
        }

        ~HttpServer() {}

    private:
        int _listensock; // 不负责通信,用来监听连接的到来,获取新连接
        uint16_t _port;
        func_t _func;
    };
}