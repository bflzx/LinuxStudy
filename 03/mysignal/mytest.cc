#include<iostream>
#include<unistd.h>
#include<sys/types.h>
using namespace std;

int main()
{
    while(true)
    {
        cout<<"我是一个正在运行的程序,pid:"<<getpid()<<endl;
        sleep(1);
    }

}