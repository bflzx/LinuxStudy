#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std::placeholders;

struct client
{
    ip::tcp::socket sock;
    streambuf buff;
};

std::vector<client> clients;

void handle_client()
{
    for(int i = 0;i < clients.size();i++)
    {
        async_read_until(clients[i].sock,clients[i].buff,'\n',std::bind(on_read,clients[i],_1,_2));
    }
    for(int i = 0;i < 10;i++)
    {
        std::thread(handle_clients_thread);
    }
}

void handle_clients_thread()
{
    // service.run();
}

void on_read(client& c,const boost::system::error_code& err,size_t read_bytes)
{
    std::istream in(&c.buff);
    std::string msg;
    std::getline(in,msg);
    if(msg == "request_login")
    {
        // c.sock.async_write("request_ok\n",on_write);
        // else if...
    }
    async_read_until(c.sock,c.buff,'\n',std::bind(on_read,c,_1,_2));
}