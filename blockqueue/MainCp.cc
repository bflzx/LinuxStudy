#include "BlockQueue.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include "Task.hpp"

//C:计算
//S:存储
template<class C,class S>
class BlockQueues
{

public:
    BlockQueue<C> *c_bq;
    BlockQueue<S> *s_bq;
};

void *consumer(void *bqs_)
{
    BlockQueue<CalTask>* bq = ((BlockQueues<CalTask,SaveTask>*)bqs_)->c_bq;
    BlockQueue<SaveTask>* save_bq = ((BlockQueues<CalTask,SaveTask>*)bqs_)->s_bq;
    while (true)
    {
        CalTask t;
        bq->pop(&t);
        std::string result = t();
        std::cout << "cal thread,完成计算任务:" << result <<"...done"<< std::endl;
        
        SaveTask save(result, Save);
        save_bq->push(save);
        std::cout << "cal thread,推送保存任务完成..."<< std::endl;
        // sleep(1);
    }
    return nullptr;
}

void *productor(void *bqs_)
{
    BlockQueue<CalTask>* bq = ((BlockQueues<CalTask,SaveTask>*)bqs_)->c_bq;
    while (true)
    {
        int x = rand() % 10 + 1;
        int y = rand() % 5;
        int operCode = rand() % oper.size();
        CalTask t(x, y, oper[operCode], mymath);
        bq->push(t);
        std::cout << "productor thread,生产计算任务:" << t.toTaskString() << std::endl;
    }
    return nullptr;
}

void* saver(void* bqs_)
{
    BlockQueue<SaveTask>* save_bq = ((BlockQueues<CalTask,SaveTask>*)bqs_)->s_bq;

    while(true)
    {
        SaveTask t;
        save_bq->pop(&t);
        t();
        std::cout << "save thread,保存任务完成..."<< std::endl;
    }
    return nullptr;
}

int main()
{
    srand((unsigned long)time(nullptr) ^ getpid());
    BlockQueues<CalTask, SaveTask> bqs;
    bqs.c_bq = new BlockQueue<CalTask>();
    bqs.s_bq = new BlockQueue<SaveTask>();

    pthread_t c, p,s;
    pthread_create(&c, nullptr, consumer,&bqs);
    pthread_create(&p, nullptr, productor,&bqs);
    pthread_create(&s, nullptr, saver, &bqs);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    pthread_join(s, nullptr);

    delete bqs.c_bq;
    delete bqs.s_bq;
    return 0;
}