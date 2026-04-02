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
#include <pthread.h>

#include "ThreadPool.hpp"
#include "Task.hpp"


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

    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(TcpServer *self, int sock) : _self(self), _sock(sock)
        {
        }

    public:
        TcpServer *_self;
        int _sock;
    };

    class TcpServer
    {
    public:
        TcpServer(const uint16_t &port = 8080)
            : _listensock(-1), _port(port)
        {
        }
        void initServer()
        {
            // 线程池初始化
            ThreadPool<Task>::getInstance()->run();
            logMessage(NORMAL, "success init success");
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
            logMessage(NORMAL, "bind socket success: %d",_listensock);

            // 设置监听状态
            if (listen(_listensock, gbacklog) < 0)
            {
                logMessage(FATAL, "listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success");
        }
        void start()
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
                logMessage(NORMAL, "accept a new link success,get new sock: %d",sock);

                // 5.未来通信,使用sock,面向字节流,后续全是文件操作
                // version 1
                //  serviceIO(sock);
                //  close(sock);

                // version 2 多进程版
                //  pid_t id = fork();
                //  if(id == 0)//child
                //  {
                //      close(_listensock);
                //      // if(fork()>0)
                //      // {
                //      //     exit(0);
                //      // }
                //      serviceIO(sock);
                //      close(sock);
                //      exit(0);
                //  }
                //  close(sock);
                //  father
                //  pid_t ret = waitpid(id,nullptr,0);
                //  if(ret > 0)
                //  {
                //      cout << "wait success: " << ret << endl;
                //  }

                // vesion 3 多线程版
                //  pthread_t tid;
                //  ThreadData* td = new ThreadData(this, sock);
                //  pthread_create(&tid, nullptr, threadRoutine, td);

                // vesion 4 线程池版
                ThreadPool<Task>::getInstance()->push(Task(sock,serviceIO));

            }
        }

        // static void *threadRoutine(void *args)
        // {
        //     pthread_detach(pthread_self());
        //     ThreadData *td = (ThreadData *)args;
        //     td->_self->serviceIO(td->_sock);     
        //     close(td->_sock);
        //     delete td;
        //     return nullptr;
        // }

        ~TcpServer()
        {
        }

    private:
        int _listensock; // 不负责通信,用来监听连接的到来,获取新连接
        uint16_t _port;
    };
}