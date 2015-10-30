# Basic TFTP

This repository contains code for basic TFTP server and client in which client **only** supports sending **read request** to server , **no write request** is implemented.

##### Compiling
Just type ``make`` to generate two executables *server* and *client*

##### Running
* To run *server* type ``./server <port>`` where 
  * port is the port number on which TFTP server will listen requests. Default port is 6500.
* To run *client* type ``./client <file_to_read> <port> <host>`` where  
  * *file_to_read* is the file tobe read by server
  * *port* is the port on which server is listening. Default port is 6500.
  * *host* is the hostname of server.Default host is localhost.

##### Basic Explanation
* TFTP client sends a file read request to TFTP server.
* To handle request server creates a child which is binded to a new socket and continues.
* Child created reads data from file if found in block of 512 bytes and sends to client otherwise it responds with an error packet.
* After receiving a reply from server client responds with acknowledge and the process continues.
* To tell the client that file has been read completely child sends a empty data packet to client which tells client to exit.
* Child retransmits data upto 3 times if it doesn't receives acknowledgement from client within an interval of 2 seconds.