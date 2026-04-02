#pragma once

#include "Thread.hpp"
#include <vector>
#include <queue>
#include <pthread.h>
#include "LockGuard.hpp"
#include <unistd.h>
#include <mutex>

using namespace ThreadNs;

const int gnum = 5;

template <class T>
class ThreadPool;

template <class T>
class ThreadData
{
public:
    ThreadPool<T> *threadpool;
    std::string name;

public:
    ThreadData(ThreadPool<T> *tp, const std::string &n) : threadpool(tp), name(n)
    {
    }
};

template <class T>
class ThreadPool
{
private:
    static void *handlerTask(void *args)
    {
        ThreadData<T> *td = (ThreadData<T> *)args;
        while (true)
        {
            T t;
            {
                LockGuard lockguadr(td->threadpool->mutex());
                while (td->threadpool->isQueueEmpty())
                {
                    td->threadpool->threadWait();
                }
                T t = td->threadpool->pop();
            }
            std::cout << td->name << " 获取了一个任务: " << t.toTaskString() << "并处理完成:" << t() << std::endl;
        }
        delete td;
        return nullptr;
    }
    ThreadPool(const int &num = gnum) : _num(num)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for (int i = 0; i < _num; i++)
        {
            _threads.push_back(new Thread());
        }
    }

    void operator=(const ThreadPool &) = delete;
    ThreadPool(const ThreadPool &) = delete;

public:
    void lockQueue() { pthread_mutex_lock(&_mutex); }
    void unlockQueue() { pthread_mutex_unlock(&_mutex); }
    bool isQueueEmpty() { return _task_queue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    T pop()
    {
        T t = _task_queue.front();
        _task_queue.pop();
        return t;
    }
    pthread_mutex_t *mutex()
    {
        return &_mutex;
    }

public:
    void run()
    {
        for (const auto &t : _threads)
        {
            ThreadData<T> *td = new ThreadData<T>(this, t->threadname());
            t->start(handlerTask, td);
            std::cout << t->threadname() << " start.." << std::endl;
        }
    }

    void push(const T &in)
    {
        LockGuard lockguadr(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for (const auto &t : _threads)
        {
            delete t;
        }
    }
    static ThreadPool<T> *getInstance()
    {
        if (tp == nullptr)
        {
            _singlock.lock();
            if (tp == nullptr)
            {
                tp = new ThreadPool<T>();
            }
            _singlock.unlock();
        }

        return tp;
    }

private:
    int _num;
    std::vector<Thread *> _threads;
    std::queue<T> _task_queue;
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    static ThreadPool<T> *tp;
    static std::mutex _singlock;
};

template <class T>
ThreadPool<T> *ThreadPool<T>::tp = nullptr;

template <class T>
std::mutex ThreadPool<T>::_singlock;