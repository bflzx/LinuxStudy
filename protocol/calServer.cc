#include "calServer.hpp"
#include <memory>

using namespace server;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << "local_port\n\n";
}

bool cal(const Request &req, Response &resp)
{
    resp.exitcode = OK;
    resp.result = 0;
    switch (req.op)
    {
    case '+':
    {
        resp.result = req.x + req.y;
        break;
    }
    case '-':
    {
        resp.result = req.x - req.y;
        break;
    }
    case '*':
    {
        resp.result = req.x * req.y;
        break;
    }
    case '/':
    {
        if (req.y == 0)
        {
            resp.exitcode = DIV_ZERO;
        }
        else
        {
            resp.result = req.x / req.y;
        }
        break;
    }
    case '%':
    {
        if (req.y == 0)
        {
            resp.exitcode = MOD_ZERO;
        }
        else
        {
            resp.result = req.x % req.y;
        }
        break;
    }
    default:
    {
        resp.exitcode = OP_ERROR;
        break;
    }
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<CalServer> tsvr(new CalServer(port));
    tsvr->initServer();
    tsvr->start(cal);
    return 0;
}