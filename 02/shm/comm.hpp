#ifndef _COMM_HPP_
#define _COMM_HPP_

#include<iostream>
#include<cerrno>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
using namespace std;

#define PATHNAME "/home/bflzx/MyLinuxStudy/02/shm"
#define PROJ_ID 0x66
#define MAX_SIZE 4096

key_t getkey()
{
    key_t k = ftok(PATHNAME,PROJ_ID);
    if(k<0)
    {
        cerr<<errno<<":"<<strerror(errno)<<endl;
        exit(1);
    }
    return k;
}

int getShmHelper(key_t k,int flags)
{
    int shmid = shmget(k,MAX_SIZE,flags);
    if(shmid < 0)
    {
        cerr<<errno<<":"<<strerror(errno)<<endl;
        exit(2);
    }
    return shmid;
}

int getShm(key_t k)
{

    return getShmHelper(k,IPC_CREAT);
}

int createShm(key_t k)
{
    return getShmHelper(k,IPC_CREAT|IPC_EXCL|0600);
}

void delShm(int shmid)
{
    if(shmctl(shmid,IPC_RMID,nullptr)==-1)
    {
        cerr<<errno<<":"<<strerror(errno)<<endl;
    }
    
}

void* attachShm(int shmid)
{
    void* mem = shmat(shmid,nullptr,0);
    if((long long)mem == -1L)
    {
        cerr<<errno<<":"<<strerror(errno)<<endl;
        exit(3);
    }
    return mem;
}

void detachShm(void* start)
{
    if(shmdt(start) == -1)
    {
        cerr<<errno<<":"<<strerror(errno)<<endl;
    }

}
#endif