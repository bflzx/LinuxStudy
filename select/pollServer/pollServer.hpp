#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <poll.h>
#include "sock.hpp"

namespace poll_ns
{
    static const int defaultport = 8080;
    static const int defaultfd = -1;
    static const int num = 1024;

    using func_t = std::function<std::string(const std::string&)>;
    class PollServer
    {
    public:
        PollServer(func_t func,int port = defaultport) :_func(func),_port(port), _listensock(-1), _rfds(nullptr)
        {
        }
        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            _rfds = new struct pollfd[num];
            for(int i = 0; i < num; ++i) ResetItem(i);
            _rfds[0].fd = _listensock;
            _rfds[0].events = POLLIN;
        }
        void Print()
        {
            std::cout << "fd list: ";
            for(int i = 0;i < num;++i)
            {
                if(_rfds[i].fd != defaultfd) std::cout << _rfds[i].fd << " ";
            }
            std::cout << std::endl;
        }
        void ResetItem(int pos)
        {
            _rfds[pos].fd = defaultfd;
            _rfds[pos].events = 0;
            _rfds[pos].revents = 0;
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
            for(;i < num;++i)
            {
                if(_rfds[i].fd != defaultfd) continue;
                else break;
            }
            if(i == num)
            {
                logMessage(WARNING,"server is full,please wait!");
            }
            else
            {
                _rfds[i].fd = sock;
                _rfds[i].events = POLLIN;
                _rfds[i].revents = 0;
            }
            Print();
        }
        void Recver(int pos)
        {
            logMessage(DEBUG,"in Recver");
            char buffer[1024];
            ssize_t s = recv(_rfds[pos].fd,buffer,sizeof(buffer)-1,0);
            if(s > 0)
            {
                buffer[s] = 0;
                std::cout << "client# " << buffer << std::endl;
            }
            else if(s == 0)
            {
                close(_rfds[pos].fd = defaultfd);
                ResetItem(pos);
                logMessage(NORMAL,"client quit");
                return;
            }
            else
            {
                close(_rfds[pos].fd = defaultfd);
                ResetItem(pos);
                logMessage(ERROR,"client error: %s",strerror(errno));
                return;
            }

            std::string response = _func(buffer);
            write(_rfds[pos].fd,response.c_str(),response.size());
        }
        void HandlerReadEvent()
        {
            for(int i = 0;i < num;++i)
            {
                if(_rfds[i].fd == defaultfd) continue;
                if(!(_rfds[i].events & POLLIN)) continue;

                if(_rfds[i].fd == _listensock && _rfds[i].revents & POLLIN) Accepter(_listensock);
                else if(_rfds[i].revents & POLLIN) Recver(i);
            }
        }

        void start()
        {
            int timeout = -1;
            for(;;)
            {
                int n = poll(_rfds,num,timeout);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "poll error,code:%d,error:%s", errno, strerror(errno));
                    break;
                default:
                    logMessage(NORMAL, "have event ready");
                    HandlerReadEvent();
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
        ~PollServer()
        {
            if(_listensock < 0) close(_listensock);
            if(_rfds) delete[] _rfds;
        }

    private:
        int _port;
        int _listensock;
        struct pollfd *_rfds;
        func_t _func;
    };
}