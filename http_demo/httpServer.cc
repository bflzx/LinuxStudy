#include "httpServer.hpp"
#include <memory>

using namespace std;
using namespace server;

void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << "port\r\n\r\n";
}

string suffixToDesc(const string suffix)
{
    string ct = "Content-Type: ";
    if(suffix == ".html")
    {
        ct += "text/html";
    }
    else if(suffix == ".jpg")
    {
        ct += "application/x-jpg";
    }
    ct += "\r\n";
    return ct;
}

bool Get(const HttpRequest &req, HttpResponse &resp)
{
    cout << "---------------------http start-----------" << endl;
    cout << req.inbuffer << endl;
    cout << "method: " << req.method << endl;
    cout << "url: " << req.url << endl;
    cout << "httpversion: " << req.httpversion << endl;
    cout << "path: " << req.path << endl;
    cout << "suffix: " << req.suffix << endl;
    cout << "size: " << req.size << "字节" << endl;
    cout << "---------------------http end-----------" << endl;
    string respline = "HTTP/1.1 200 OK\r\n";
    string respheader = suffixToDesc(req.suffix);
    respheader += "Set-Cookie: 1234567\r\n";
    // if(req.size >0 )
    // {
    // respheader += "Content-Length: ";
    // respheader += to_string(req.size);
    // respheader += "\r\n";
    // }
    string respblank = "\r\n";
    //string body = "<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>for test</title><h1>hello world</h1></head><body><p>测试网页</p></body></html>";
    string body;
    if(!Util::readFile(req.path, &body))
    {
        Util::readFile(html_404, &body);
    }
    
    resp.outbuffer += respline;
    resp.outbuffer += respheader;
    resp.outbuffer += respblank;
    cout << "----------------http response start--------------" << endl;
    cout << resp.outbuffer << endl;
    cout << "----------------http response end----------------" << endl;
    resp.outbuffer += body;

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<HttpServer> httpsvr(new HttpServer(Get, port));
    httpsvr->initServer();
    httpsvr->start();
    return 0;
}