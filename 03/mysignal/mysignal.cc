#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<cstdlib>

using namespace std;


static void Usage(const string & proc)
{
    cout<<"\nUsage: "<<proc<<" pid signo\n"<<endl;
}



int main(int argc,char* argv[])
{
    alarm(1);
    int cnt = 0;
    while(true)
    {
        cout<<cnt++<<endl;
    }

    // if(argc!=3)
    // {
    //     Usage(argv[0]);
    //     exit(1);
    // }
    // pid_t pid = atoi(argv[1]);
    // int signo = atoi(argv[2]);
    // int n = kill(pid,signo);
    // if(n != 0)
    // {
    //     perror("kill");
    // }


    // while(true)
    // {
    //     cout<<"我是一个进程:"<<getpid()<<endl;
    //     sleep(1);
    // }
    return 0;
}