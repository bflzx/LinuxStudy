#include"comm.hpp"


int main()
{
    cout<<"client begin"<<endl;
    int wfd = open(NAMED_PIPE,O_WRONLY);
    cout<<"client end"<<endl;

    if(wfd<0)
    {
        exit(1);
    }    

    char buffer[1024];
    while(true)
    {
        cout<<"Please Say# ";
        fgets(buffer,sizeof(buffer),stdin);
        buffer[strlen(buffer)-1] = 0;
        ssize_t n = write(wfd,buffer,strlen(buffer));
        (void)n;
    }

    close(wfd);
    return 0;
}