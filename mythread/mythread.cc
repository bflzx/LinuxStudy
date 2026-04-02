#include<iostream>
#include<cstdio>
#include<cstring>
#include<string>
#include<unistd.h>
#include<pthread.h>
#include<memory>
#include"Thread.hpp"

using namespace std;

void* thread_run(void* args)
{
    string work_type = (const char*)args;
    while(true)
    {
        cout<<"我是一个新线程,我正在做:"<<work_type<<endl;
        sleep(1);
    }
}

int main()
{
    unique_ptr<Thread> thread(new Thread(thread_run,(void*)"hellothread",0));
    thread->join(); 
    
}