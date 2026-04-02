#include <iostream>
#include <string>
#include <functional>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std::placeholders;

void connect_handler(const boost::system::error_code& ec,ip::tcp::socket &sock)
{
    if(!ec)
    {
        std::cout << "连接成功! 可以开始接收数据!" << std::endl;

        std::string msg;
        std::cin >> msg;
        
        sock.write_some(buffer(msg));
    }
}

int main()
{
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("192.168.195.134"),8080);
    ip::tcp::socket sock(service);
    sock.async_connect(ep,std::bind(connect_handler,_1,std::ref(sock)));
    service.run();
    return 0;
}