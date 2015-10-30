//
// Created by anchit on 28/10/15.
//

#include "include.h"

int main(int argc,char * argv[])
{
    int port=1001;

    if(argc>2)
        err("Invaid arguments");
    if(argc==2)
    {
        port=atoi(argv[1]);
        if(port<0)
            err("Invalid port entered\n");
    }
    if(argc==1)
    {
        port=6500;
        printf("No port entered.Using default port 6500\n");
    }

    start_server(port);
    return 0;
}
