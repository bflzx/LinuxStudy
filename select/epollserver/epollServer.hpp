#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <functional>
#include "err.hpp"
#include "log.hpp"
#include "sock.hpp"

namespace epoll_ns
{
    static const int defaultport = 8080;
    static const int size = 128;
    static const int defaultvalue = -1;
    static const int defaultnum = 64;

    using func_t = std::function<std::string (const std::string&)>;
    class EpollServer
    {
    public:
        EpollServer(func_t f,int num = defaultnum,uint16_t port = defaultport):func_(f),_num(num),_revs(nullptr),_port(port),_listensock(defaultvalue),_epfd(defaultvalue)
        {}

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock,_port);
            Sock::Listen(_listensock);
            
            _epfd = epoll_create(size);
            if(_epfd < 0)
            {
                logMessage(FATAL,"epoll create error:%s",strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }

            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listensock;
            epoll_ctl(_epfd,EPOLL_CTL_ADD,_listensock,&ev);

            _revs = new struct epoll_event[_num];

            logMessage(NORMAL,"init server sucess");
        }
        void HandlerEvent(int readyNum)
        {
            logMessage(DEBUG,"HandlerEvent in");
            for(int i = 0;i < readyNum;++i)
            {
                uint32_t events = _revs[i].events;
                int sock = _revs[i].data.fd;

                if(sock == _listensock && (events & EPOLLIN))
                {
                    std::string clientip;
                    uint16_t clientport;
                    int fd = Sock::Accept(sock,&clientip,&clientport);
                    if(fd < 0)
                    {
                        logMessage(WARNING,"accept error");
                        continue;
                    }
                    
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);
                }
                else if(events & EPOLLIN)
                {
                    char buffer[1024];
                    int n = recv(sock,buffer,sizeof(buffer)-1,0);
                    if(n > 0)
                    {
                        buffer[n] = 0;
                        logMessage(DEBUG,"client# %s",buffer);

                        std::string response = func_(buffer);

                        send(sock,response.c_str(),response.size(),0);
                    }
                    else if(n == 0)
                    {
                        epoll_ctl(_epfd,EPOLL_CTL_DEL,sock,nullptr);
                        close(sock);
                        logMessage(NORMAL,"client quit");
                    }
                    else
                    {
                        epoll_ctl(_epfd,EPOLL_CTL_DEL,sock,nullptr);
                        close(sock);
                        logMessage(ERROR,"recv error, code:%d ,errstring:%s",errno,strerror(errno));
                    }
                }
                else
                {

                }
            }
            logMessage(DEBUG,"HandlerEvent out");
        }
        void start()
        {
            int timeout = 1000;
            for(;;)
            {
                int n = epoll_wait(_epfd,_revs,_num,timeout);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL,"timeout...");
                    break;
                case -1:
                    logMessage(WARNING,"epoll_wait failed, code:%d,errstring:%s",errno,strerror(errno));
                    break;
                default:
                    logMessage(NORMAL,"have event ready");
                    HandlerEvent(n);
                    break;
                }
            }
        }

        ~EpollServer()
        {
            if(_listensock != defaultvalue) close(_listensock);
            if(_epfd != defaultvalue) close(_epfd);
            if(_revs) delete []_revs;
        }
    private:
        uint16_t _port;
        int _listensock;
        int _epfd;
        struct epoll_event *_revs;
        int _num;
        func_t func_;
    };
}