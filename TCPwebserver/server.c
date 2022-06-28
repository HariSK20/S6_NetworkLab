#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"proxy_header_S1934.h"

int main()
{
	// IP and port to use
	char *ip = "127.0.0.2";
	int port = SERVER_PORT;

	// The required data structures for handling the addresses
	struct sockaddr_in server_addr, client_addr;

	// Socket file descriptors
	int server_sock, client_sock;
	socklen_t addr_size;
	int n;
	
	// The buffer using which we pass data
	char buffer[1024];

	// memset(buffer, 0, 1024);

	// Create a socket for IPv4 Address Family, and a Bidirectional TCP stream
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0)
	{
		perror(" !> Unable to create socket\n");
		exit(-1);
	}
	printf(" > Socket Created!\n");

	// Setting up the server address
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);

	// Binding the address to socket
	n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0)
	{
		perror(" !> Unable to bind socket\n");
		exit(-1);
	}
	printf(" > Socket Binding complete!\n");

	// Start listening for incomming connections
	listen(server_sock, 5);
	printf(" > Listening!\n");

	addr_size = sizeof(client_addr);
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
	printf(" >> Client Connected!\n");

	// Receive data
	bzero(buffer, 1024);
	recv(client_sock, buffer, 1024, 0);
	printf(" >> Received from client: %s\n", buffer);


	unsigned long l = 0;
	do
	{
		// Accept an incoming connection

		// Receive data
		bzero(buffer, 1024);
		recv(client_sock, buffer, 1024, 0);
		printf(" >> Received from client: %s\n", buffer);
		l = strtol(buffer, NULL, 10);

		sleep(SLEEP_TIME);
		// send data
		bzero(buffer, 1024);
		sprintf(buffer, "%lu", l+1);
		send(client_sock, buffer, strlen(buffer), 0);
		printf(" >> Sent response: %s\n", buffer);

		// close the connection
	}while(l < TIMEOUT);
	strcpy(buffer, "\\QUIT");
	send(client_sock, buffer, strlen(buffer), 0);

	close(client_sock);
	printf(" >> Client Disconnected\n");

	return(0);
}
