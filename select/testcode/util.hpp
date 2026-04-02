#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

void setNonBlock(int fd)
{
    int f1 = fcntl(fd, F_GETFL);
    if(f1 < 0)
    {
        std::cerr << "fcntl: " << strerror(f1) << std::endl;
        return;
    }
    fcntl(fd, F_SETFL, f1 | O_NONBLOCK);
}

void printLog()
{
    std::cout << "this is a log" << std::endl;
}

void download()
{
    std::cout << "this is a download" << std::endl;
}

void executeSql()
{
    std::cout << "this is a executeSql" << std::endl;
}