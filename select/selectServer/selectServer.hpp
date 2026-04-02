#pragma once

#include <iostream>
#include <string>
#include <functional>
#include "sock.hpp"

namespace select_ns
{
    static const int defaultport = 8080;
    static const int fdnum = sizeof(fd_set) * 8;
    static const int defaultfd = -1;

    using func_t = std::function<std::string(const std::string&)>;
    class SelectServer
    {
    public:
        SelectServer(func_t f,int port = defaultport) :func(f),_port(port), _listensock(-1), fdarray(nullptr)
        {
        }
        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            fdarray = new int[fdnum];
            for(int i = 0; i < fdnum; ++i) fdarray[i] = defaultfd;
            fdarray[0] = _listensock;
        }
        void Print()
        {
            std::cout << "fd list: ";
            for(int i = 0;i < fdnum;++i)
            {
                if(fdarray[i] != defaultfd) std::cout << fdarray[i] << " ";
            }
            std::cout << std::endl;
        }
        void Accepter(int listensock)
        {
            std::string clientip;
            uint16_t clientport;
            int sock = Sock::Accept(_listensock, &clientip, &clientport);
            if (sock < 0)
                return;
            logMessage(NORMAL, "accept success[%s:%d]", clientip.c_str(), clientport);
            int i = 0;
            for(;i < fdnum;++i)
            {
                if(fdarray[i] != defaultfd) continue;
                else break;
            }
            if(i == fdnum)
            {
                logMessage(WARNING,"server is full,please wait!");
            }
            else
            {
                fdarray[i] = sock;
            }
            Print();
        }
        void Recver(int sock,int pos)
        {
            logMessage(DEBUG,"in Recver");
            char buffer[1024];
            ssize_t s = recv(sock,buffer,sizeof(buffer)-1,0);
            if(s > 0)
            {
                buffer[s] = 0;
                std::cout << "client# " << buffer << std::endl;
            }
            else if(s == 0)
            {
                close(sock);
                fdarray[pos] = defaultfd;
                logMessage(NORMAL,"client quit");
                return;
            }
            else
            {
                close(sock);
                fdarray[pos] = defaultfd;
                logMessage(ERROR,"client error: %s",strerror(errno));
                return;
            }

            std::string response = func(buffer);
            write(sock,response.c_str(),response.size());
        }
        void HandlerReadEvent(fd_set &rfds)
        {
            for(int i = 0;i < fdnum;++i)
            {
                if(fdarray[i] == defaultfd) continue;

                if(FD_ISSET(fdarray[i], &rfds) && fdarray[i] == _listensock) Accepter(_listensock);
                else if(FD_ISSET(fdarray[i],&rfds)) Recver(fdarray[i],i);
            }
        }

        void start()
        {
            for(;;)
            {
                fd_set rfds;
                // fd_set wfds;
                FD_ZERO(&rfds);
                int maxfd = fdarray[0];
                for(int i = 0; i < fdnum; ++i)
                {
                    if (fdarray[i] == defaultfd) continue;
                    FD_SET(fdarray[i], &rfds);
                    if (maxfd < fdarray[i]) maxfd = fdarray[i];
                }

                logMessage(NORMAL,"maxfd is:%d",maxfd);
                // struct timeval timeout = {1, 0};
                // int n = select(_listensock + 1, &rfds, nullptr, nullptr, &timeout);
                int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "select error,code:%d,error:%s", errno, strerror(errno));
                    break;
                default:
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent(rfds);
                    // HandWriteEvent(wfds);
                    break;  
                }
                // sleep(1);
                // std::string clientip;
                // uint16_t clientport;
                // int sock = Sock::Accept(_listensock, &clientip, &clientport);
                // if (sock < 0) continue;
            }
        }
        ~SelectServer()
        {
            if(_listensock < 0) close(_listensock);
            if(fdarray) delete[] fdarray;
        }

    private:
        int _port;
        int _listensock;
        int *fdarray;
        func_t func;
    };
}