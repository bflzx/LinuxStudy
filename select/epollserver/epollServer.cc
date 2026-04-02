#include "epollServer.hpp"
#include <memory>

using namespace std;
using namespace epoll_ns;

// ./epoll_server port
static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << "port" << "\n\n";
}

std::string echo(const std::string& message)
{
    return "I am epollserver," + message;
}

int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<EpollServer> epollsvr(new EpollServer(echo,port));

    epollsvr->initServer();
    epollsvr->start();
    return 0;
}