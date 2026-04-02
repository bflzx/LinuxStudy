#pragma

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "protocol.hpp"

#define NUM 1024

using namespace std;

class CalClient
{
public:
    CalClient(const string &serverip, const uint16_t &serverport)
        : _sock(-1), _serverip(serverip), _serverport(serverport)
    {
    }
    void initClient()
    {
        // 1.创建套接字
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (_sock < 0)
        {
            cerr << "socket create error" << endl;
            exit(2);
        }
        // 2.bind,不需要显示bind
    }
    void start()
    {
        // 3.connnect
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(_serverport);
        server.sin_addr.s_addr = inet_addr(_serverip.c_str());

        if (connect(_sock, (struct sockaddr *)&server, sizeof(server)) != 0)
        {
            cerr << "socket connect error" << endl;
        }
        else
        {
            string line;
            string inbuffer;
            while (true)
            {
                cout << "Enter>>> ";
                getline(cin, line);
                Request req = ParseLine(line);
                string content;
                req.serialize(&content);
                string send_string = enLength(content);
                send(_sock, send_string.c_str(), send_string.size(), 0);

                string package, text;
                if (!recvPackage(_sock, inbuffer, &package))
                {
                    continue;
                }
                if (!deLength(package, &text))
                {
                    continue;
                }
                Response resp;
                resp.deserialize(text);
                cout << "exitcode: " << resp.exitcode << endl;
                cout << "result: " << resp.result << endl;
            }
        }
    }

    Request ParseLine(const string &line)
    {
        int status = 0; // 0:操作符之前,1:碰到了操作符,2:操作符之后
        int i = 0;
        int cnt = line.size();
        string left, right;
        char op;
        while (i < cnt)
        {
            switch (status)
            {
            case 0:
            {
                if (!isdigit(line[i]))
                {
                    op = line[i];
                    status = 1;
                }
                else
                {
                    left.push_back(line[i++]);
                }
                break;
            }
            case 1:
            {
                i++;
                status = 2;
                break;
            }
            case 2:
            {
                right.push_back(line[i++]);
                break;
            }
            }
        }
        return Request(stoi(left), stoi(right), op);
    }

    ~CalClient()
    {
        if (_sock >= 0)
        {
            close(_sock);
        }
    }

private:
    int _sock;
    string _serverip;
    uint16_t _serverport;
};