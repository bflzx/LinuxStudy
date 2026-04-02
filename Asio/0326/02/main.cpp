#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <boost/asio.hpp>

using namespace boost::asio;

struct client
{
    ip::tcp::socket sock;
    char buffer[1024];
    int already_read;

    bool set_reading()
    {
        std::lock_guard<std::mutex> lk(_cs);
        if(_is_reading) return false; // 已经在读取
        else
        {
            _is_reading = true;
            return true;
        }
    }

    void unset_reading()
    {
        std::lock_guard<std::mutex> lk(_cs);
        _is_reading = false;
    }
private:
    std::mutex _cs;
    bool _is_reading;
};

std::vector<client> clients; 

void handle_clients()
{
    for(int i = 0;i < 10;i++)
    {
        std::thread(handle_clients_thread);
    }
}

void handle_clients_threads()
{
    while(true)
    {
        for(int i = 0;i < clients.size();i++)
        {
            if(clients[i].sock.available())
            {
                if(clients[i].set_reading())
                {
                    on_read(clients[i]);
                    clients[i].unset_reading();
                }
            }
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