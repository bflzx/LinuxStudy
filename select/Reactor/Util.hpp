#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>

class Util
{
public:
    static bool SetNonBlock(int fd)
    {
        int f1 = fcntl(fd,F_GETFL);
        if(f1 < 0) return false;
        fcntl(fd,F_SETFL,f1|O_NONBLOCK);
        return true;
    }
};