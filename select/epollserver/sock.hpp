#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.hpp"
#include "err.hpp"


class Sock
{
    const static int backlog = 32;
public:
    static int Socket()
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0)
        {
            logMessage(FATAL, "create socket error");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "create socket success:%d", sock);

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR,&opt,sizeof(opt));
        return sock;
    }

    static void Bind(int sock,int port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind socket error");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket sucess");
    }

    static void Listen(int sock)
    {
        if(listen(sock,backlog) < 0)
        {
            logMessage(FATAL, "listen socket error");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success");
    }

    static int Accept(int listensock,std::string *clientip,uint16_t* clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listensock, (struct sockaddr *)&peer, &len);
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
};