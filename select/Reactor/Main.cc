#include <memory>
#include "TcpServer.hpp"

using namespace tcpserver;

static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " port" << "\n\t";
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

void calculate(Connection *conn)
{
    std::string onePackage;
    while(ParseOnePackage(conn->inbuffer_,&onePackage))
    {
        std::string reqStr;
        if(!deLength(onePackage,&reqStr)) return;
        std::cout << "去掉报文的正文: \n" << reqStr << std::endl;

        Request req;
        if(!req.deserialize(reqStr)) return;

        Response resp;
        cal(req,resp);

        std::string respStr;
        resp.serialize(&respStr);
        conn->outbuffer_ += enLength(respStr);

        std::cout << "result: " << conn->outbuffer_ << std::endl;
        if(conn->sender_) conn->sender_(conn);
    }
}


int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    
    std::unique_ptr<TcpServer> tsvr(new TcpServer(calculate,port));
    tsvr->InitServer();
    tsvr->Dispatcher();
    return 0;
}