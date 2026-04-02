#include<iostream>
#include<unistd.h>
#include<cstdlib>
#include<vector>
#include<string>
#include<ctime>
#include<sys/types.h>
#include<sys/wait.h>
using namespace std;

#define Make_Rand() srand((unsigned long)time(nullptr)^getpid())
#define PROCSS_NUM 5
typedef void(*func_t)();

void downLoadTask()
{
    cout<<getpid()<<":下载任务"<<endl;
    sleep(1);
}
void ioTask()
{
    cout<<getpid()<<":IO任务"<<endl;
    sleep(1);
}
void flushTask()
{
    cout<<getpid()<<":刷新任务"<<endl;
    sleep(1);
}

void loadTaskFunc(vector<func_t>* out)
{
    out->push_back(downLoadTask);
    out->push_back(ioTask);
    out->push_back(flushTask);
}


class subEp
{
public:
    subEp(pid_t subId,int writeFd)
    :subId_(subId)
    ,writeFd_(writeFd)
    {
        char nameBuffer[1024];
        snprintf(nameBuffer,sizeof(nameBuffer),"process-%d[(%d)-%d]",num++,subId,writeFd_);
        name_ = nameBuffer;
    }

public:
    static int num;
    string name_;
    pid_t subId_;
    int writeFd_;


};
int subEp::num = 0;

int recvTask(int readFd)
{
    int code = 0;
    ssize_t s = read(readFd,&code,sizeof(code));
    if(s == 4)
    {
        return code;
    }
    else
    {
        return -1;
    }
}

void createSubProcess(vector<subEp> *subs,vector<func_t> &funcMap)
{
 

    for(int i = 0;i<PROCSS_NUM;i++)
    {
        int fds[2];
        int n = pipe(fds);
        (void)n;
        pid_t id = fork();
        if(id == 0)
        {
            //子进程,进行处理任务
            close(fds[1]);
            while(true)
            {
                //1.获取命令码，如果没有发送，子进程应该阻塞
                int commandCode = recvTask(fds[0]);
                //2.完成任务
                if(commandCode >=0 &&commandCode<funcMap.size())
                {
                    funcMap[commandCode]();
                }
                else if(commandCode == -1)
                {
                    break;
                }
            }
            exit(0);
        }
        close(fds[0]);
        subEp sub(id,fds[1]);
        subs->push_back(sub);
    }
}

void sendTask(const subEp &process,int taskNum)
{
    cout<<"send task num:"<<taskNum<<"send to:"<<process.name_<<endl;
    int n = write(process.writeFd_,&taskNum,sizeof(taskNum));
    (void)n;
}

void loadBlanceContrl(const vector<subEp>& subs,const vector<func_t>& funcMap,int count)
{
    int processnum = subs.size();
    int tasknum = funcMap.size();
    bool forever = (count == 0?true:false);
    while(true)
    {
        //1.选择一个子进程
        int subIdx = rand()%processnum;
        //2.选择一个任务
        int taskIdx = rand()%tasknum;
        //3.任务发送给选择的进程
        sendTask(subs[subIdx],taskIdx);

        sleep(1);
        if(!forever)
        {
            count--;
            if(count == 0)
            {
                break;
            }
        }
    }
    for(int i = 0;i<processnum;i++)
    {
        close(subs[i].writeFd_);
    }

}

void waitProcess(vector<subEp>& processes)
{
    int processnum = processes.size();
    for(int i = 0;i<processnum;i++)
    {
        waitpid(processes[i].subId_,nullptr,0);
        cout<<"wait sub process sucess ...."<<processes[i].subId_<<endl;
    }
}

int main()
{
    Make_Rand();
    //1.建立子进程并建立和子进程通信的通道
    vector<subEp> subs;
    vector<func_t> funcMap;
    loadTaskFunc(&funcMap);
    createSubProcess(&subs,funcMap);
    //2.走到这里的是父进程,控制子进程
    int taskCnt = 20;//0.永远进行 >0.父进程运行Cnt次
    loadBlanceContrl(subs,funcMap,taskCnt);
    
    //3.回收子进程信息
    waitProcess(subs);
    return 0;
}