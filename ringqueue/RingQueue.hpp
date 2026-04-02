#pragma once

#include<iostream>
#include<vector>
#include<semaphore.h>
#include<cassert>

static const int gcap = 5;

template<class T>
class RingQueue
{
private:
    void P(sem_t& sem)
    {
        int n = sem_wait(&sem);
        assert(n == 0);
        (void)n;
    }
    void V(sem_t& sem)
    {
        int n = sem_post(&sem);
        assert(n == 0);
        (void)n;
    }

public:
    RingQueue(const int& cap = gcap)
    :_queue(cap),_cap(cap)
    {
        int n = sem_init(&_spaceSem,0,_cap);
        assert(n == 0);
        n = sem_init(&_dataSem,0,0);
        assert(n == 0);

        _productorStep = _consumerStep = 0;
    }

    void push(const T& in)
    {
        P(_spaceSem);
        _queue[_productorStep++] = in;
        _productorStep %= _cap;
        V(_dataSem);
    }

    void pop(T* out)
    {
        P(_dataSem);
        *out = _queue[_consumerStep++];
        _consumerStep %= _cap;
        V(_spaceSem);
    }

    ~RingQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);
    }

private:
    std::vector<T> _queue;
    int _cap;
    sem_t _spaceSem; //生产者,空间资源
    sem_t _dataSem; //消费者,数据资源
    int _productorStep;
    int _consumerStep;
};