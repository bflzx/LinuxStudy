#pragma once

#include <iostream>
#include <string>
#include<ctime>
#include<unistd.h>
#include<cstdarg>

#define NUM 1024

#define LOG_NORMAL "log.txt"
#define LOG_ERR "log.error"

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char *to_levelstr(int level)
{
    switch (level)
    {
    case DEBUG:
    {
        return "DEBUG";
    }
    case NORMAL:
    {
        return "NORMAL";
    }
    case WARNING:
    {
        return "WARNING";
    }
    case ERROR:
    {
        return "ERROR";
    }
    case FATAL:
    {
        return "FATAL";
    }
    default:
    {
        return nullptr;
    }
    }
}

void logMessage(int level, const char *format, ...)
{
    //[日志等级] [时间戳/时间] [pid] [message]
    char logprefix[NUM];
    snprintf(logprefix, sizeof(logprefix), "[%s][%ld][pid:%d]",to_levelstr(level),(long int)time(nullptr),getpid());

    char logcontent[NUM];
    va_list arg;
    va_start(arg, format);

    vsnprintf(logcontent, sizeof(logcontent), format, arg);

    // std::cout << logprefix << logprefix << std::endl;

    FILE *log = fopen(LOG_NORMAL, "a");
    FILE *err = fopen(LOG_ERR, "a");
    if(log != nullptr && err != nullptr)
    {
        FILE *cur = nullptr;
        if (level == DEBUG || level == NORMAL || level == WARNING)
        {
            cur = log;
        }
        if(level == ERROR || level == FATAL)
        {
            cur = err;
        }
        if(cur)
        {
            fprintf(cur, "%s%s\n", logprefix, logcontent);
        }
        fclose(log);
        fclose(err);
    }
}