#pragma once

#include <iostream>
#include <pthread.h>
#include <string>
#include <functional>
#include <cstring>
#include<cassert>

using namespace std;

class Thread;

class Context
{
public:
    Thread *this_;
    void *args_;

public:
    Context() : this_(nullptr), args_(nullptr)
    {
    }
    ~Context()
    {
    }
};

class Thread
{
public:
    typedef std::function<void *(void *)> func_t;
    const int num = 1024;

public:
    Thread(func_t func, void *args, int number) : func_(func), args_(args)
    {
        char buffer[num];
        snprintf(buffer, sizeof(buffer), "thread-%d", number);
        name_ = buffer;
        Context *ctx = new Context;
        ctx->this_ = this;
        ctx->args_ = args_;
        int n = pthread_create(&tid_, nullptr, start_routine, args_);
        assert(n == 0);
        (void)n;
    }
    static void *start_routine(void *args) // 类内成员,有缺省参数this
    {
        // 静态方法不能调用成员方法或成员变量，只能调用静态的
        Context *ctx = (Context *)(args);
        void *ret = ctx->this_->run(ctx->args_);
        delete ctx;
        return ret;
    }

    void join()
    {
        int n = pthread_join(tid_, nullptr);
        assert(n == 0);
        (void)n;
    }
    void *run(void *args)
    {
        return func_(args);
    }
    ~Thread()
    {
    }

private:
    string name_;
    pthread_t tid_;
    func_t func_;
    void *args_;
};