#include "include.h"

void receive_data(int socketfd,char* fname)
{
    struct sockaddr_in server_address;
    int nbytes,add_len=sizeof(struct sockaddr);
    int block=0,nextblock=1;

    file_data *fdata=(file_data *)malloc(sizeof(file_data));
    ack_data *ack=(ack_data *)malloc(sizeof(ack_data));

    while(1)
    {
        if (sigsetjmp(endbuf, 1) != 0)
        {
            break;
        }

        memset((void *)fdata,0,sizeof(file_data));
        memset((void *)ack,0,sizeof(ack_data));

        signal(SIGINT, timer_handler);
        signal(SIGALRM, timer_handler);

        printf("Waiting for DATA...\n\n");

        nbytes = recvfrom(socketfd, fdata, sizeof(file_data), 0, (struct sockaddr *)&server_address, &add_len);
        if(nbytes<0)
        {
            printf("Error in receiving data");
            break;
        }

        int data_length=strlen(fdata->data);
        if(data_length==0)
            break;

        printf("Received DATA for block #%d\n\n", fdata->data_block);
        fprintf(stderr,"%s\n\n",fdata->data);
        block=nextblock;
        nextblock++;
        ack->data_block=block;

        sigsetjmp(timeoutbuf, 1);

        printf("Sending ACK for block #%d      ", block);
        nbytes = sendto(socketfd, ack, sizeof(ack_data), 0, (struct sockaddr *)&server_address, (socklen_t)add_len);

        if(nbytes<0)
        {
            perror("Error in sending ACK");
            break;
        }
        printf("ACK sent\n\n\n");
        if(data_length<BUFFLEN)
            break;
    }
    if(fdata->data_block==-1)
    {
        perror("File does not exists\n");
        exit(1);
    }
}

void send_request(char* host,int port,char *fname)
{
    int nbytes,socketfd,addr_len=sizeof(struct sockaddr_in);
    struct sockaddr_in server_address,client_address;
    char *comp_req;

    socketfd=create_socket();
    fill_address(&server_address,port,host);
    fill_address(&client_address,0,NULL);
    bind_socket(&client_address,socketfd);

    ack_data *ack=(ack_data *)malloc(sizeof(ack_data));
    memset(ack,0,sizeof(ack_data));

    request *req=(request *)malloc(sizeof(request));
    req->type=FIL;
    strcpy(req->filename,fname);

    comp_req=(char *)malloc(BUFFLEN);
    compress_req(req,comp_req);

    sigsetjmp(timeoutbuf,1);
    printf("Sending read request to server         ");

    nbytes=sendto(socketfd,comp_req,strlen(comp_req),0,(struct sockaddr *)&server_address,addr_len);
    if(nbytes<0)
        err("Error in sending request to server");

    receive_data(socketfd,fname);
}

void start_client(int port,char* host,char *fname)
{
    send_request(host,port,fname);
}