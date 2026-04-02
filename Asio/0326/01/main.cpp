#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using namespace boost::asio;

struct client
{
    ip::tcp::socket sock;
    char buffer[1024];
    int already_read;
};

std::vector<client> clients;

void handle_clients()
{
    while(true)
    {
        for(int i = 0;i < clients.size();i++)
        {
            if(clients[i].sock.available()) on_read(clients[i]);
        }
    }
}

void on_read(client& c)
{
    int to_read = std::min(1024-c.already_read,(int)c.sock.available());
    c.sock.read_some(buffer(c.buffer + c.already_read,to_read));
    c.already_read += to_read;

    if(std::find(c.buffer,c.buffer + c.already_read,'\n') < c.buffer + c.already_read)
    {
        int pos = std::find(c.buffer,c.buffer + c.already_read,'\n') - c.buffer;
        std::string msg(c.buffer,c.buffer+pos);
        std::copy(c.buffer+pos,c.buffer+1024,c.buffer);
        c.already_read -= pos;
        on_read_msg(c,msg);
    }
}

void on_read_msg(client& c,const std::string& msg)
{
    //分析消息,然后返回
    if(msg == "request_login")
    {
        c.sock.write_some(buffer("request_ok\n"));
    }
    // else if(...)...
}