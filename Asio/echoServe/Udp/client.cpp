#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost::asio;

io_service service;
ip::udp::endpoint ep(ip::address::from_string("192.168.195.134"),8080);

void sync_echo(std::string msg)
{
    ip::udp::socket sock(service);
    sock.open(ip::udp::v4());
    sock.send_to(buffer(msg),ep);
    char buff[1024];
    ip::udp::endpoint sender_ep;
    int bytes = sock.receive_from(buffer(buff),sender_ep);
    std::string copy(buff,bytes);
    std::cout << "server echoed our " << msg << ": " << (copy == msg ?
    "OK" : "FAIL") << std::endl;
    sock.close();
}

int main(int argc,char* argv[])
{
    char* messages[] = {"John says hi","so does James","Lucy got home", 0 };
    boost::thread_group threads;
    for(char** message = messages;*message;++message)
    {
        threads.create_thread(boost::bind(sync_echo,*message));

        boost::this_thread::sleep(boost::posix_time::millisec(100));
    }
    return 0;
}