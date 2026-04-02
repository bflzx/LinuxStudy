#include"tcpServer.hpp"
#include<memory>

int main(int argc,char* argv[])
{
    if(argc != 2)
    {
        cerr << "请输入端口" << endl;
        exit(-1);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<TcpServer> tsvr(new TcpServer(port));
    tsvr->init();
    tsvr->start();
    
    return 0;
}