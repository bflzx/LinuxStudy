#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;

int main()
{
    io_service service;
    ip::tcp::resolver res(service);
    ip::tcp::resolver::query query("www.whtcc.edu.cn","80");
    ip::tcp::resolver::iterator it = res.resolve(query);
    while(it != ip::tcp::resolver::iterator())
    {
        ip::tcp::endpoint ep = *it;
        std::cout << "ip: " << ep.address() << " 端口: " << ep.port() << std::endl;
        it++;
    }
    
    return 0;
}