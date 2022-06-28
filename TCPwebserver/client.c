#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"proxy_header_S1934.h"

int main()
{
	// IP and port to use
	char *ip = "127.0.0.2"	;
	int port = CLIENT_PORT;

	// The required data structures for handling the addresses
	struct sockaddr_in server_addr;
	int sock, n;
	socklen_t addr_size;
	char buffer[1024];

	// Create a socket for IPv4 Address Family, and a Bidirectional TCP stream
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror(" !> Unable to create socket\n");
		exit(-1);
	}
	printf(" > Socket Created!\n");

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);

	// Connect to the server address
	n = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0)
	{
		perror(" !> Unable to connect to server!");
		exit(1);
	}
	printf(" > Connected to server\n");
	bzero(buffer, 1024);
	recv(sock, buffer, 1024, 0);

	unsigned long l = 0;
	do
	{
		// send data
		bzero(buffer, 1024);
		sprintf(buffer, "%lu", l+1);
		send(sock, buffer, strlen(buffer), 0);
		printf(" >> Sent string: %s\n", buffer);

		sleep(SLEEP_TIME);
		// receive data
		bzero(buffer, 1024);
		recv(sock, buffer, 1024, 0);
		printf(" >> Got response: %s\n", buffer);
		l = strtol(buffer, NULL, 10);
	}while(l < TIMEOUT);
	strcpy(buffer, "\\QUIT");
	send(sock, buffer, strlen(buffer), 0);

	// close connection
	close(sock);
	printf(" > Disconnected from server\n");

	return(0);
}
