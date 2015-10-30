#include "include.h"

void data_send(int socket_fd,struct sockaddr *client_addr, socklen_t client_len, char *filename)
{
    int fsize,nbytes,pos=0, nextAck = -1;
    int block=0;
    int fd=open(filename,O_RDONLY);
    char buff[BUFFLEN];

    if(fd==-1)
        err("Error in opening file");

    fsize=lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    file_data *fdata=(file_data *)malloc(sizeof(file_data));
    ack_data *ackdata=(ack_data *)malloc(sizeof(file_data));

    memset((void*)fdata,0,sizeof(file_data));
    memset((void*)ackdata,0,sizeof(ack_data));
    memset((void*)buff,'\0',BUFFLEN);

    while (pos<fsize)
    {
        nextAck=-1;
        nbytes=read(fd,buff,BUFFLEN);
        if(nbytes==-1)
            err("Error in reading file");

        pos+=nbytes;
        block++;

        fdata->data_block=block;
        memcpy(fdata->data,buff,nbytes);

        printf("Sending block %d\n",block);

        sigsetjmp(timeoutbuf, 1);
        nbytes=sendto(socket_fd,fdata,nbytes+4,0,(struct sockaddr *)client_addr,client_len);
        if(nbytes<0)
        {
            fprintf(stderr,"Error in sendng data for block %d\n",block);
            break;
        }


        if(sigsetjmp(endbuf,1)!=0)
            break;

        signal(SIGINT,timer_handler);
        signal(SIGALRM,timer_handler);

        while(nextAck<0)
        {
            printf("Waiting for ACK     ");
            alarm(3);
            nbytes=recvfrom(socket_fd,ackdata,sizeof(ack_data),0,client_addr,&client_len);
            if(nbytes<0)
            {
                fprintf(stderr,"Error in receing ack for block %d\n",block);
                sleep(10);
                break;
            }
            timeout=0;
            alarm(0);
//            printf("ACK block %d received and block =%d\n",ackdata->data_block,block);
            if (ackdata->data_block == block)
            {
                nextAck = 1;
                if (pos == fsize)
                {
                    printf("Received ACK for final block #%d\n\n", block);
                    break;
                }
                else
                {
                    printf("Received ACK for block #%d\n\n", block);
                    continue;
                }
            }
            else if (ntohs(ackdata->data_block) < block)
            {
                nextAck = -1;
            }
        }
    }
    memset((void*)buff,'\0',BUFFLEN);
    fdata->data_block=block;
    memcpy(fdata->data,buff,sizeof(buff));
    sendto(socket_fd,fdata,sizeof(buff)+4,0,client_addr,client_len);
    close(socket_fd);
}

void create_child(int prev_socket_fd,struct sockaddr *client_address, request *client_req)
{
    pid_t pid=fork();
    if(pid<0)
        err("Error in creating child");
    else if(pid == 0)
    {
        close(prev_socket_fd);

        int socket_fd,addr_len=sizeof(struct sockaddr);
        struct sockaddr_in child_address;

        socket_fd=create_socket();
        fill_address(&child_address,0,NULL);
        bind_socket(&child_address,socket_fd);

        if (getsockname(socket_fd, (struct sockaddr *)&child_address, (socklen_t *)&addr_len) == -1)
            err("Error in resolving child address");

        printf("Child process created.Listening on port %d\n\n",ntohs(child_address.sin_port));

        ack_data *ack=(ack_data *)malloc(sizeof(ack_data));
        memset((void *)ack,0,sizeof(ack_data));

        if(!existence(client_req->filename))
        {
            perror("File does not exists");
            printf("File name is %s\n",client_req->filename );
            file_data *fdata=(file_data *)malloc(sizeof(file_data));
            fdata->data_block=-1;
            ssize_t nbytes=sendto(socket_fd,fdata,sizeof(file_data),0,client_address,(socklen_t )addr_len);

            exit(-1);
        }

        data_send(socket_fd,client_address,addr_len,client_req->filename);

        close(socket_fd);
        exit(0);
    }
}

void start_server(int port)
{
    signal(SIGCHLD,sigchld_handler);

    int client_length,socket_fd;
    struct sockaddr_in server_address,client_address;
    char *buf;
    ssize_t nbytes;

    socket_fd=create_socket();
    fill_address(&server_address,port,NULL);
    bind_socket(&server_address,socket_fd);

    printf("Server listening on port %i \n",ntohs(server_address.sin_port));

    while(1)
    {
        buf=(char *)malloc(sizeof(request));
        memset(buf,0,sizeof(request));
        client_length=sizeof(client_address);

        nbytes=recvfrom(socket_fd,buf,BUFFLEN,0,(struct sockaddr *)&client_address,&client_length);
        if(nbytes<0)
            err("Error in receiving request from client\n");
//        receive_message(socket_fd,buf,&client_address);

        fprintf(stderr,"\n\nReceived packet from client %s:%d\n",inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        fflush(stdin);
        request *client_req=(request *)malloc(sizeof(request));
        decompress(client_req,buf);

        create_child(socket_fd,(struct sockaddr *)&client_address, client_req);
    }

    close(socket_fd);
}
