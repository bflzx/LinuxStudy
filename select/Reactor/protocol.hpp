#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>

#define SEP " "
#define SEP_LEN strlen(SEP)
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

enum
{
    OK = 0,
    DIV_ZERO,
    MOD_ZERO,
    OP_ERROR
};

//"x op y" ->"content_len"\r\n"x op y"\r\n
//"exitcode result"->"content-len"\r\n"exitcode result"\r\n
const std::string enLength(const std::string &text)
{
    std::string send_string = std::to_string(text.size());
    send_string += LINE_SEP;
    send_string += text;
    send_string += LINE_SEP;
    return send_string;
}

bool deLength(const std::string &package, std::string *text)
{
    auto pos = package.find(LINE_SEP);
    if (pos == std::string::npos)
    {
        return false;
    }
    std::string text_len_string = package.substr(0, pos);
    int text_len = std::stoi(text_len_string);
    *text = package.substr(pos + LINE_SEP_LEN, text_len);
    return true;
}

class Request
{
public:
    Request() : x(0), y(0), op(0)
    {
    }
    Request(int x_, int y_, char op_) : x(x_), y(y_), op(op_)
    {
    }
    bool serialize(std::string *out)
    {
        //"x op y"
        *out = "";
        std::string x_string = std::to_string(x);
        std::string y_string = std::to_string(y);

        *out = x_string;
        *out += SEP;
        *out += op;
        *out += SEP;
        *out += y_string;
        return true;
    }
    bool deserialize(const std::string &in)
    {
        auto left = in.find(SEP);
        auto right = in.rfind(SEP);
        if (left == std::string::npos || right == std::string::npos)
        {
            return false;
        }
        if (left == right)
        {
            return false;
        }
        if (right - (left + SEP_LEN) != 1)
        {
            return false;
        }

        std::string x_string = in.substr(0, left);
        std::string y_string = in.substr(right + SEP_LEN);
        if (x_string.empty() || y_string.empty())
        {
            return false;
        }
        x = std::stoi(x_string);
        y = std::stoi(y_string);
        op = in[left + SEP_LEN];

        return true;
    }

public:
    int x;
    int y;
    char op;
};

class Response
{
public:
    Response() : exitcode(0), result(0)
    {
    }
    Response(int exitcode_, int result_) : exitcode(exitcode_), result(result_)
    {
    }
    bool serialize(std::string *out)
    {
        *out = "";
        std::string ec_string = std::to_string(exitcode);
        std::string res_string = std::to_string(result);

        *out = ec_string;
        *out += SEP;
        *out += res_string;
        return true;
    }
    bool deserialize(const std::string &in)
    {
        auto mid = in.find(SEP);
        if (mid == std::string::npos)
        {
            return false;
        }
        std::string ec_string = in.substr(0, mid);
        std::string res_string = in.substr(mid + SEP_LEN);
        if (ec_string.empty() || res_string.empty())
        {
            return false;
        }
        exitcode = std::stoi(ec_string);
        result = std::stoi(res_string);

        return true;
    }

public:
    int exitcode; // 0:计算成功 !0:计算失败
    int result;   // 计算结果
};

bool ParseOnePackage(std::string &inbuffer, std::string *text)
{
    *text = "";
    auto pos = inbuffer.find(LINE_SEP);
    if (pos == std::string::npos) return false;
    std::string text_len_string = inbuffer.substr(0, pos);
    int text_len = std::stoi(text_len_string);
    int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len;

    std::cout << "处理前#inbuffer: \n" << inbuffer << std::endl;
    if (inbuffer.size() < total_len) return false;
    // 到这至少有一个完整的报文
    *text = inbuffer.substr(0, total_len);
    inbuffer.erase(0, total_len);
    return true;
}
