#include <iostream>
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

int main()
{
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("192.168.195.134"),8080);
    ip::tcp::socket sock(service);
    
    boost::system::error_code ec;
    sock.connect(ep,ec);

    if(!ec)
    { 
        std::cout << "连接服务器成功!" << std::endl;

        std::string msg;
        std::cin >> msg;
        sock.write_some(buffer(msg),ec);
    }
    else std::cout << "连接服务器失败:" << ec.message() << std::endl;
    return 0;
}