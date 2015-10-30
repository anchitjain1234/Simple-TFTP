 
#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <setjmp.h>

#define BUFFLEN 512

#define FIL 1
#define ACK 2
#define ERR 3

jmp_buf timeoutbuf, endbuf;
int timeout;

typedef struct{
    int type;
    char filename[200];
} request;

typedef struct{
    int data_block;
    char data[BUFFLEN];
} file_data;

typedef struct{
    int data_block;
} ack_data;

void err(char *message);
void sigchld_handler(int s);
int existence(char *fname);
int create_socket();
void fill_address(struct sockaddr_in *address,int port,char *host);
void bind_socket(struct sockaddr_in *address,int sock);
void timer_handler(int sig);
void decompress(request* req,char *buff);
void compress_req(request* req,char *buff);
void receive_data(int socketfd,char* fname);
void send_request(char* host,int port,char *fname);
void start_client(int port,char* host,char *fname);
void data_send(int socket_fd,struct sockaddr *client_addr, socklen_t client_len, char *filename);
void create_child(int prev_socket_fd,struct sockaddr *client_address, request *client_req);
void start_server(int port);

#endif //INCLUDE_H