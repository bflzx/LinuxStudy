#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

io_service service;

void handle_connect()
{
    char buff[1024];
    ip::udp::socket sock(service,ip::udp::endpoint(ip::udp::v4(),8080));
    while(true)
    {
        ip::udp::endpoint sender_ep;
        int bytes = sock.receive_from(buffer(buff),sender_ep);
        std::string msg(buff,bytes);
        sock.send_to(buffer(msg),sender_ep);
    }
}

int main(int argc,char* argv[])
{
    handle_connect();
    return 0;
}