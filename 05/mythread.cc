#include<iostream>
#include<unistd.h>
#include<pthread.h>
using namespace std;

void* thread_routine(void* args)
{
    const char* name = (const char*)args;
    while(true)
    {
        cout<<"我是一个新线程,我正在运行!name:"<<name<<endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    int n = pthread_create(&tid,nullptr,thread_routine,(void*)"thread one");
    while(true)
    {
    
        cout<<"我是主线程,我正在运行!我创建出的线程的tid:"<<tid<<endl;
        sleep(1);
    }
    return 0;
}
