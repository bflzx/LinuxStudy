#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

namespace Client
{
    using namespace std;
    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    class udpClient
    {
    public:
        udpClient(const string &serverip, const uint16_t &serverport) : _sockfd(-1), _serverip(serverip), _serverport(serverport), _quit(false)
        {
        }
        void initClient()
        {
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (_sockfd == -1)
            {
                cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
                exit(2);
            }
            cout << "socket success:" << ":" << _sockfd << endl;
            pthread_mutex_init(&_mutex, nullptr);
        }

        static void *readMessage(void *args)
        {
            int sockfd = *((int *)args);
            pthread_detach(pthread_self());

            while (true)
            {
                char resoponse_message[1024];
                struct sockaddr_in temp;
                socklen_t temp_len = sizeof(temp);
                int n = recvfrom(sockfd, resoponse_message, sizeof(resoponse_message) - 1, 0, (struct sockaddr *)&temp, &temp_len);

                if (n >= 0)
                {
                    pthread_mutex_lock(&_mutex);
                    resoponse_message[n] = 0;
                    cout << resoponse_message << endl;
                    cout << "Please Enter#";
                    fflush(stdout);
                    pthread_mutex_unlock(&_mutex);
                }
            }
            return nullptr;
        }

        void run()
        {
            pthread_create(&_reader, nullptr, readMessage, &_sockfd);

            struct sockaddr_in server;
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverip.c_str());
            server.sin_port = htons(_serverport);

            string message;
            char cmdline[1024];

            while (!_quit)
            {
                pthread_mutex_lock(&_mutex);
                cout << "Please Enter#";
                fflush(stdout);
                pthread_mutex_unlock(&_mutex);
                // cin >> message;
                fgets(cmdline, sizeof(cmdline), stdin);
                cmdline[strlen(cmdline) - 1] = 0;
                message = cmdline;
                if(message == "quit")
                {
                    _quit = true;
                }
                sendto(_sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&server, sizeof(server));
            }
        }
        ~udpClient()
        {
        }

    private:
        int _sockfd;
        string _serverip;
        uint16_t _serverport;
        bool _quit;

        pthread_t _reader;
    };
}