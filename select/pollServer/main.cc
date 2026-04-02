#include "pollServer.hpp"
#include <memory>

using namespace std;
using namespace poll_ns;

// ./select_server 8081
static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << "port" << "\n\n";
}

std::string transaction(const std::string& request)
{
    return request;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    unique_ptr<PollServer> svr(new PollServer(transaction,atoi(argv[1])));

    svr->initServer();
    svr->start();
    return 0;
}