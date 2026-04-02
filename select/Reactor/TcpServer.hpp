#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <cassert>
#include "Sock.hpp"
#include "Log.hpp"
#include "Err.hpp"
#include "Epoller.hpp"
#include "Util.hpp"
#include "protocol.hpp"

namespace tcpserver
{
    class Connection;
    class TcpServer;
    static const uint16_t defaultport = 8080;
    static const int num = 64;

    using func_t = std::function<void (Connection*)>;
    using hander_t = std::function<void (const std::string& package)>;

    class Revent
    {
    public:
        int sock;
        uint32_t revents;
    };

    class Connection
    {
    public:
        Connection(int sock):sock_(sock),tsp_(nullptr)
        {}
        void Register(func_t r,func_t s,func_t e)
        {
            recver_ = r;
            sender_ = s;
            excepter_ = e;
        }
        ~Connection()
        {}
    public:
        int sock_;
        std::string inbuffer_;
        std::string outbuffer_;

        func_t recver_;
        func_t sender_;
        func_t excepter_;

        TcpServer *tsp_;
    };

    class TcpServer
    {
    private:
        void Recver(Connection *conn)
        {
            char buffer[1024];
            while(true)
            {
                ssize_t s = recv(conn->sock_,buffer,sizeof(buffer)-1,0);
                if(s > 0)
                {
                    buffer[s] = 0;
                    conn->inbuffer_ += buffer;
                }                
                else if(s == 0)
                {
                    if(conn->excepter_) conn->excepter_(conn);
                }
                else
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                    else if(errno == EINTR) continue;
                    else
                    {
                        if(conn->excepter_) 
                        {
                            conn->excepter_(conn);
                            break;
                        }
                    }
                }
            }
            service_(conn);
        }
        void Sender(Connection *conn)
        {
            while(true)
            {
                ssize_t s = send(conn->sock_,conn->outbuffer_.c_str(),conn->outbuffer_.size(),0);
                if(s > 0)
                {
                    if(conn->outbuffer_.empty()) break;
                    else
                    {
                        conn->outbuffer_.erase(0,s);
                    }
                }
                else
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                    else if(errno == EINTR) continue;
                    else
                    {
                        if(conn->excepter_) 
                        {
                            conn->excepter_(conn);
                            break;
                        }
                    }
                }
            }
        }
        void Excepter(Connection *conn)
        {

        }
        void Accepter(Connection* conn)
        {
            for(;;)
            {
                std::string clientip;
                uint16_t clientport;
                int err = 0;
                int sock = sock_.Accept(&clientip,&clientport,&err);
                if(sock > 0)
                {
                    AddConnection(sock,EPOLLIN | EPOLLET,
                                        std::bind(&TcpServer::Recver,this,std::placeholders::_1),
                                        std::bind(&TcpServer::Sender,this,std::placeholders::_1),
                                        std::bind(&TcpServer::Excepter,this,std::placeholders::_1)
                    );
                    logMessage(DEBUG,"get a new link,info:[%s:%d]",clientip.c_str(),clientport);
                }
                else
                {
                    if(err == EAGAIN || err == EWOULDBLOCK) break;
                    else if(err == EINTR) continue;
                    else
                    {
                        break;
                    }
                }
            }
        }
        void AddConnection(int sock,uint32_t events,func_t recver,func_t sender,func_t excepter)
        {
            if(events & EPOLLIN) Util::SetNonBlock(sock_.Fd());
            Connection *conn = new Connection(sock);
            conn->Register(recver,sender,excepter);

            bool r = epoller_.AddEvent(sock,EPOLLIN|EPOLLET);
            assert(r);
            (void)r;
            connections_.insert(std::make_pair(sock,conn));
            logMessage(DEBUG,"add new sock : %d in epoll and unordered_map",sock);
        }
        bool IsConnectionExists(int sock)
        {
            auto iter = connections_.find(sock);
            return iter != connections_.end();
        }
        void Loop(int timeout)
        {
            int n = epoller_.Wait(revs_,num_,timeout);
            for(int i = 0;i < n;i++)
            {
                int sock = revs_[i].data.fd;
                uint32_t events = revs_[i].events;

                //将所有的异常问题转化为读写问题
                if(events & EPOLLERR) events |= (EPOLLIN|EPOLLOUT);
                if(events & EPOLLHUP) events |= (EPOLLIN|EPOLLOUT);


                if(IsConnectionExists(sock) && (events & EPOLLIN) && connections_[sock]->recver_)
                {
                    connections_[sock]->recver_(connections_[sock]);
                }
                if(IsConnectionExists(sock) && (events & EPOLLOUT) && connections_[sock]->sender_)
                {
                    connections_[sock]->sender_(connections_[sock]);
                }
            }
        }
    public:
        TcpServer(func_t func,uint16_t port = defaultport):service_(func),port_(port),revs_(nullptr)
        {}
        void InitServer()
        {
            sock_.Socket();
            sock_.Bind(port_);
            sock_.Listen();

            epoller_.Create();
            AddConnection(sock_.Fd(),EPOLLIN|EPOLLET,std::bind(&TcpServer::Accepter,this,std::placeholders::_1),nullptr,nullptr);
            revs_ = new struct epoll_event[num];
            num_ = num;
        }

        void Dispatcher()
        {
            int timeout = -1;
            while(true)
            {
                Loop(timeout);
                logMessage(DEBUG,"time out...");
            }
        }
        ~TcpServer()
        {
            sock_.Close();
            epoller_.Close();
            if(revs_ != nullptr) delete []revs_;
        }
    private:
        uint16_t port_;
        Sock sock_;
        Epoller epoller_;
        std::unordered_map<int,Connection*> connections_;
        struct epoll_event *revs_;
        int num_;
        // hander_t handler_;
        func_t service_;
    };
}