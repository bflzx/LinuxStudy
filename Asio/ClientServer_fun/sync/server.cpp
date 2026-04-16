#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;
using namespace boost::placeholders;

io_service service;

void accept_thread()
{
    ip::tcp::acceptor acceptor(service,ip::tcp::endpoint(ip::tcp::v4(),8080));
    while(true)
    {
        
    }
}