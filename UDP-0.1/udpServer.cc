#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>
#include "onlineUser.hpp"

using namespace std;
using namespace Server;

const string dictTxt = "./dict.txt";

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << "local_port\n\n";
}

static bool cutString(const string &target, string *s1, string *s2, const string &sep)
{
    auto pos = target.find(sep);
    if (pos == string::npos)
    {
        return false;
    }
    *s1 = target.substr(0, pos);
    *s2 = target.substr(pos + sep.size(), target.size());
    return true;
}

unordered_map<string, string> dict;
static void initDict()
{
    ifstream in(dictTxt, ios::binary);
    if (!in.is_open())
    {
        cerr << "open file " << dictTxt << " error" << endl;
        exit(OPEN_ERR);
    }
    string line;
    string key, value;
    while (getline(in, line))
    {
        // cout << line << endl;
        if (cutString(line, &key, &value, ":"))
        {
            dict.insert(make_pair(key, value));
        }
    }
    in.close();
}

void reload(int signo)
{
    (void)signo;
    initDict();
}

void handlerMessage(int sockfd, string clientip, uint16_t clientport, string message)
{
    auto iter = dict.find(message);
    string response_message;
    if (iter == dict.end())
    {
        response_message = "unknown";
    }
    else
    {
        response_message = iter->second;
    }

    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(clientport);
    client.sin_addr.s_addr = inet_addr(clientip.c_str());

    sendto(sockfd, response_message.c_str(), response_message.size(), 0, (struct sockaddr *)&client, sizeof(client));
}

void execCommand(int sockfd, string clientip, uint16_t clientport, string cmd)
{
    string response;
    FILE *fp = popen(cmd.c_str(), "r");
    if (fp == nullptr)
    {
        response = cmd + " exec filed";
    }
    char line[1024];
    while (fgets(line, sizeof(line), fp))
    {
        response += line;
    }
    pclose(fp);

    struct sockaddr_in client;
    bzero(&client, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(clientport);
    client.sin_addr.s_addr = inet_addr(clientip.c_str());

    sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr *)&client, sizeof(client));
}

OnlineUser onlineuser;

void routeMessage(int sockfd, string clientip, uint16_t clientport, string message)
{
    if (!onlineuser.isOnline(clientip, clientport))
    {
        onlineuser.addUser(clientip, clientport);

        struct sockaddr_in client;
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(clientport);
        client.sin_addr.s_addr = inet_addr(clientip.c_str());
        string response = "已自动上线，可发送消息进行广播";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr *)&client, sizeof(client));
        return;
    }

    if (message == "offline")
    {
        onlineuser.delUser(clientip, clientport);
        struct sockaddr_in client;
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(clientport);
        client.sin_addr.s_addr = inet_addr(clientip.c_str());
        string response = "已下线";
        sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr *)&client, sizeof(client));
        return;
    }

    onlineuser.broadcastMessage(sockfd, clientip, clientport, message);
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    // string ip = argv[1];

    // signal(2, reload);
    // initDict();

    // std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage,port));
    // std::unique_ptr<udpServer> usvr(new udpServer(execCommand,port));
    std::unique_ptr<udpServer> usvr(new udpServer(routeMessage,port));

    usvr->initServer();
    usvr->start();
    return 0;
}