#pragma once

#include<iostream>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string>
#include<cerrno>
#include<cstring>
#include<unistd.h>
using namespace std;

#define NAMED_PIPE "/home/bflzx/MyLinuxStudy/01/named_pipe/my_pipe"

bool createFifo(const string& path)
{
    umask(0);
    int n = mkfifo(path.c_str(),0600);
    if(n == 0)
    {
        return true;
    }
    else
    {
        cout<<"errno"<<errno<<"err  string: "<<strerror(errno)<<endl;
        return false;
    }
}


void removeFifo(const string& path)
{
    int n = unlink(path.c_str());
    (void)n;
}

