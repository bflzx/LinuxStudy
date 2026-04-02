#include<iostream>
#include<signal.h>
#include<unistd.h>
#include<vector>
using namespace std;

//#define BLOCK_SIGNAL 2
#define MAX_SIGNUM 31

static vector<int> sigarr = {2};

static void show_pending(const sigset_t& pending)
{
    for(int signo = MAX_SIGNUM;signo>=1;signo--)
    {
        if(sigismember(&pending,signo))
        {
            cout<<"1";
        }
        else
        {
            cout<< "0";
        }
    }
    cout<<endl;
}


int main()
{
    //1.先屏蔽指定信号
    sigset_t block,oblock,pending;
    //1.1初始化
    sigemptyset(&block);
    sigemptyset(&oblock);
    sigemptyset(&pending);
    //1.2添加要屏蔽的信号
    for(const auto sig:sigarr)
    {
        sigaddset(&block,sig);
    }
    // sigaddset(&block,BLOCK_SIGNAL);
    //1.3开始屏蔽
    sigprocmask(SIG_SETMASK,&block,&oblock);

    //2.遍历打印pengding信号集
    int cnt = 10;
    while(true)
    {
        //2.1初始化
        sigemptyset(&pending);
        //2.2获取它
        sigpending(&pending);
        //2.3打印它
        show_pending(pending);
        sleep(1);
        if(cnt-- == 0)
        {
            sigprocmask(SIG_SETMASK,&oblock,&block);
            cout<<"恢复对信号的屏蔽,不屏蔽任何信号"<<endl;
        }
    }
    return 0;
}