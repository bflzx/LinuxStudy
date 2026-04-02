#include"RingQueue.hpp"
#include<pthread.h>
#include<ctime>
#include<cstdlib>
#include<sys/types.h>
#include<unistd.h>
#include"Task.hpp"

void* ProductorRoutine(void* rq)
{
    RingQueue<Task> *ringqueue = (RingQueue<Task> *)rq;
    while(true)
    {
        int x = rand() % 10;
        int y = rand() % 5;
        char op = oper[rand() % oper.size()];
        Task t(x, y, op, mymath);
        ringqueue->push(t);
        std::cout << "生产任务:" << t.toTaskString() << std::endl;
    }
    return nullptr;
}

void* ConsumerRoutine(void* rq)
{
    RingQueue<Task> *ringqueue = (RingQueue<Task> *)rq;
    while(true)
    {
        Task t;
        ringqueue->pop(&t);
        std::string result = t();
        std::cout << "消费任务:" << result << std::endl;
    }
    return nullptr;
}

int main()
{
    srand((unsigned int)time(nullptr)^getpid()^pthread_self());
    RingQueue<Task> *rq = new RingQueue<Task>();

    pthread_t c, p;
    pthread_create(&p, nullptr, ProductorRoutine, rq);
    pthread_create(&c, nullptr, ConsumerRoutine, rq);

    pthread_join(p, nullptr);
    pthread_join(c, nullptr);
    delete rq;
    return 0;
}