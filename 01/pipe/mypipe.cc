#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>
#include<cstring>
using namespace std;

int main()
{
   
    int fds[2];
    int n = pipe(fds);
    
    pid_t id  = fork();
    if(id == 0)//子进程
    {
        //子进程写入
        close(fds[0]);
        //string msg = "Hello ,i m child";
        const char* s = "我是子进程,我正在给你发消息";
        int cnt = 0;
        while(true)
        {
            cnt++;
            char buffer[1024];
            snprintf(buffer,sizeof(buffer),"child->parent say:%s[%d][%d]",s,cnt,getpid());
            write(fds[1],buffer,strlen(buffer));
            sleep(1);
        }


        exit(0);
    }
    else//父进程
    {
        //父进程读取
        close(fds[1]);
        
        while(true)
        {
            char buffer[1024];
            ssize_t s = read(fds[0],buffer,sizeof(buffer)-1);
            if(s > 0)
            {
                buffer[s] = 0;
            }
            cout<<"Get Message#"<<buffer<<" | my pid"<<getpid()<<endl;
        }

        int status = 0;
        n = waitpid(id,&status,0);
    }


    // cout<<"fds[0]:"<<fds[0]<<endl;
    // cout<<"fds[1]:"<<fds[1]<<endl;

    return 0;
}
