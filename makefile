all:server client clean

server:server.o utilities.o server_functions.o
	gcc -g server.o utilities.o server_functions.o -o server
client:client.o utilities.o client_functions.o
	gcc -g client.o utilities.o client_functions.o -o client
server.o:server.c
	gcc -g -c server.c -o server.o
server_functions.o:server_functions.c
	gcc -g -c server_functions.c -o server_functions.o
client.o:client.c
	gcc -g -c client.c -o client.o
client_functions.o:client_functions.c
	gcc -g -c client_functions.c -o client_functions.o
utilities.o:utilities.c
	gcc -g -c utilities.c -o utilities.o
clean:
	rm *.o
