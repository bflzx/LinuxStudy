#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include "Err.hpp"

const static int backlog = 32;
const static int defaultsock = -1;

class Sock
{
public:
    Sock():listensock_(-1)
    {}
    ~Sock()
    {
        if(listensock_ != defaultsock) close(listensock_);
    }
public:
    void Socket()
    {
        listensock_ = socket(AF_INET, SOCK_STREAM, 0);
        if(listensock_ < 0)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success:%d", listensock_);

        int opt = 1;
        setsockopt(listensock_, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR,&opt,sizeof(opt));
    }

    void Bind(int port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if(bind(listensock_,(struct sockaddr*)&local,sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind socket error");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket sucess");
    }

    void Listen()
    {
        if(listen(listensock_,backlog) < 0)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    int Accept(std::string *clientip,uint16_t* clientport,int *err)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listensock_, (struct sockaddr *)&peer, &len);
        *err = errno;
        if(sock < 0)
        {
            logMessage(ERROR, "accept error,next");
        }
        else
        {
            logMessage(NORMAL, "accept a new link success,get new sock:%d", sock);
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }

    int Fd()
    {
        return listensock_;
    }
    void Close()
    {
        if(listensock_ != defaultsock) close(listensock_);
    }
private:
    int listensock_;
};