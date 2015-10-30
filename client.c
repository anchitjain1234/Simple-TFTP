//
// Created by anchit on 28/10/15.
//

#include "include.h"
int main(int argc,char * argv[])
{
    int port=1001;
    char host[100],fname[200];

    if(argc>4 || argc==1)
        err("Invaid arguments.Usage: client <file> <port> <host>\n");
    strcpy(fname,argv[1]);
    if(argc==4)
    {
        port=atoi(argv[2]);
        strcpy(host,argv[3]);
        if(port<0)
            err("Invalid port entered\n");
    }
    if(argc==3)
    {
        port=atoi(argv[2]);
        strcpy(host,"localhost");
        if(port<0)
            err("Invalid port entered\n");
        printf("No host entered.Using localhost\n");
    }
    if(argc==2)
    {
        port=6500;
        strcpy(host,"localhost");
        printf("No host and port entered.Using localhost and 6500 respectively.\n");
    }

    start_client(port,host,fname);
    return 0;
}


