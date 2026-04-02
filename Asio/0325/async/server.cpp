#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std::placeholders;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

io_service service;
ip::tcp::endpoint ep(ip::tcp::v4(),8080);
ip::tcp::acceptor acc(service,ep);

void start_accept(socket_ptr sock);
void handler_accept(const boost::system::error_code& ec,socket_ptr sock);

void start_accept(socket_ptr sock)
{
    acc.async_accept(*sock,std::bind(handler_accept,_1,std::ref(sock)));
}

void handler_accept(const boost::system::error_code& ec,socket_ptr sock)
{
    if(!ec)
    {
        std::cout << "链接到来!" << std::endl;

        char recv_msg[1024] = {0};
        
        sock->read_some(buffer(recv_msg));
        std::cout << "客户端:" << recv_msg << std::endl;

        socket_ptr new_sock(new ip::tcp::socket(service));

        start_accept(new_sock);
    }
}

int main()
{
    socket_ptr sock(new ip::tcp::socket(service));
    start_accept(sock);

    service.run();
    return 0;
}
