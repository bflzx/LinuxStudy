#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

void client_session(socket_ptr sock)
{
    try
    {
        while(true)
        {
            char data[512] = {0};
            size_t len = sock->read_some(buffer(data));

            if(len > 0)
            {
                std::cout << "收到: " << data << std::endl;
                write(*sock,buffer("ok",2));
            }
        }
    }
    catch(...)
    {}
}

int main()
{
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(),8080);
    ip::tcp::acceptor acc(service,ep);
    
    std::cout << "服务器启动,端口: 8080" << std::endl;

    while(true)
    {
        socket_ptr sock(new ip::tcp::socket(service));
        acc.accept(*sock);

        std::cout << "客户端接入! " << std::endl;
        std::thread t(client_session,sock);
        t.detach();
    }
    return 0;
}