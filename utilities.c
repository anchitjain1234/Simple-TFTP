#include "include.h"

void err(char *message)
{
    perror(message);
    exit(-1);
}

void sigchld_handler(int s) {
    int pid = wait(NULL);
    if (pid < 0) {
        return;
    }
}

void timer_handler(int sig) {
    switch(sig) {
        case SIGALRM: {
            timeout++;
            if (timeout >= 3) {
                printf("Retransmission timed out.\n");
                timeout = 0;
                alarm(0);
                longjmp(endbuf, sig);
            }
            ("Retransmitting.\n");
            longjmp(timeoutbuf, sig);
        } break;
        case SIGINT: {
            printf("Transfer interrupted.\n");
            timeout = 0;
            alarm(0);
            longjmp(endbuf, sig);
        } break;
        default: break;
    }
}

int existence(char *fname)
{
    if(access(fname,F_OK)!=-1)
        return 1;
    return 0;
}

int create_socket()
{
    int sck;
    if ((sck=socket(AF_INET,SOCK_DGRAM,0)) == -1)
        err("server socket error");
    return sck;
}

void fill_address(struct sockaddr_in *address,int port,char *host)
{
    memset((void *)address,0,sizeof(*address));
    address->sin_port=htons(port);
    address->sin_family=AF_INET;

    if(host!=NULL)
    {
        struct hostent *host_ip = gethostbyname(host);
        if(host_ip==NULL)
            err("Error in getting host from hostname");
        address->sin_addr = *((struct in_addr *)host_ip->h_addr);
    }
    else
    {
        address->sin_addr.s_addr = htonl(INADDR_ANY);
    }
}

void bind_socket(struct sockaddr_in *address,int sock)
{
    if(bind(sock,(struct sockaddr *)address,sizeof(*address)) == -1)
        err("server binding error");
}

void decompress(request* req,char *buff)
{
    int buff_len=strlen(buff),i;
//    char data[BUFFLEN];
    if(buff_len>=3 && buff[0]=='F' && buff[1]=='I' && buff[2]=='L')
    {
        req->type=FIL;
        for(i=3;i<buff_len;i++)
        {
            req->filename[i-3]=buff[i];
        }
    }
    else
    {
        req->type=ERR;
        return;
    }
}

void compress_req(request* req,char *buff)
{
    int i;
    if(req->type==FIL)
    {
        strcpy(buff,"FIL");
        for(i=0;i<strlen(req->filename);i++)
        {
            buff[i+3]=req->filename[i];
        }
    }
    else
    {
        strcpy(buff,"ERR");
    }
}