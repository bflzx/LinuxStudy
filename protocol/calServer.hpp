#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include "log.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include "protocol.hpp"
#include <functional>

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

    using func_t = function<bool(const Request &req, Response &resp)>;

    void handlerEntery(int sock, func_t func)
    {
        std::string inbuffer;
        while (true)
        {
            // 1.读取
            std::string req_text, req_str;
            if (!recvPackage(sock, inbuffer, &req_text)) // 保证req_text一定是完整的请求
            {
                return;
            }
            std::cout << "带报头的请求:\n"<< req_text << std::endl;
            if (!deLength(req_text, &req_str))
            {
                return;
            }
            std::cout << "去掉报头的正文:\n"<< req_str << std::endl;
            // 2.反序列化
            Request req;
            if (!req.deserialize(req_str))
            {
                return;
            }
            // 3.计算任务
            Response resp;
            func(req, resp);
            // 4.序列化
            std::string resp_str;
            resp.serialize(&resp_str);
            std::cout << "计算完成,序列化响应:" << resp_str << std::endl;
            // 5.发送响应
            std::string send_string = enLength(resp_str);
            std::cout << "构建完成完整的响应\n"<< send_string << std::endl;
            send(sock, send_string.c_str(), send_string.size(), 0);
        }
    }

    class CalServer
    {
    public:
        CalServer(const uint16_t &port = 8080)
            : _listensock(-1), _port(port)
        {
        }
        void initServer()
        {
            // 1.创建socket套接字
            _listensock = socket(AF_INET, SOCK_STREAM, 0);
            if (_listensock < 0)
            {
                logMessage(FATAL, "create socket error");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "create socket success");

            // 2.bind
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                logMessage(FATAL, "bind socket error");
                exit(BIND_ERR);
            }
            logMessage(NORMAL, "bind socket success: %d", _listensock);

            // 设置监听状态
            if (listen(_listensock, gbacklog) < 0)
            {
                logMessage(FATAL, "listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success");
        }
        void start(func_t func)
        {
            // signal(SIGCHLD, SIG_IGN);
            for (;;)
            {
                // 4.server获取新链接
                // sock,和client通信的fd
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listensock, (struct sockaddr *)&peer, &len);
                if (sock < 0)
                {
                    logMessage(ERROR, "accept error,next");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success,get new sock: %d", sock);

                pid_t id = fork();
                if (id == 0) // child
                {
                    close(_listensock);
                    // if(fork()>0)
                    // {
                    //     exit(0);
                    // }
                    // serviceIO(sock);
                    handlerEntery(sock, func);
                    close(sock);
                    exit(0);
                }
                close(sock);
                // father
                pid_t ret = waitpid(id, nullptr, 0);
                if (ret > 0)
                {
                    logMessage(NORMAL, "wait child success");
                }
            }
        }

        ~CalServer() {}

    private:
        int _listensock; // 不负责通信,用来监听连接的到来,获取新连接
        uint16_t _port;
    };
}