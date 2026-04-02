#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <cstdio>

const std::string oper = "+-*/%";

void serviceIO(int sock)
{
    char buffer[1024];
    while (true)
    {
        ssize_t n = read(sock, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "recv message: " << buffer << std::endl;

            std::string outbuffer = buffer;
            outbuffer += " server[echo]";

            write(sock, outbuffer.c_str(), outbuffer.size());
        }
        else if (n == 0)
        {
            // 代表client退出,断开连接
            logMessage(NORMAL, "client quit,me too!");
            break;
        }
    }
    close(sock);
}

class Task
{
    using func_t = std::function<void(int)>;

public:
    Task()
    {
    }
    Task(int sock, func_t func)
        :_sock(sock), _callback(func)
    {
    }
    void operator()()
    {
        _callback(_sock);
    }

private:
    int _sock;
    func_t _callback;
};

int mymath(int x, int y, char op)
{
    int result = 0;
    switch (op)
    {
    case '+':
        result = x + y;
        break;
    case '-':
        result = x - y;
        break;
    case '*':
        result = x * y;
        break;
    case '/':
    {
        if (y == 0)
        {
            std::cerr << "div zero error!" << std::endl;
            result = -1;
        }
        else
        {
            result = x / y;
        }
        break;
    }
    case '%':
        if (y == 0)
        {
            std::cerr << "mod zero error!" << std::endl;
            result = -1;
        }
        else
        {
            result = x % y;
        }
        break;
    default:
        break;
    }

    return result;
}
