#include"comm.hpp"

int main()
{
    key_t k = getkey();
    printf("0x%x\n",k);
    int shmid = createShm(k);

    char* start = (char*)attachShm(shmid);
    printf("attach success,address start:%p\n",start);

    while(true)
    {
        printf("client say : %s\n",start);
        sleep(1);
    }


    detachShm(start);
    delShm(shmid);
    return 0;
}