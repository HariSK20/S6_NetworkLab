#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<time.h>
// typedef struct Message
// {

// }Message;
#define TIMEOUT 10


int main()
{
	// IP and port to use
	char *ip = "127.0.0.2";
	int port = 5566;

	// The required data structures for handling the addresses
	struct sockaddr_in server_addr, client_addr;

	// Socket file descriptors
	int server_sock;
	socklen_t addr_size;
	int n;

	// time_t start_t;
	
	// The buffer using which we pass data
	char buffer[1024], text[1024];

	// memset(buffer, 0, 1024);

	// Create a socket for IPv4 Address Family, and a Bidirectional UDP DGRAM
	server_sock = socket(AF_INET, SOCK_DGRAM, 0);
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
	printf(" > Listening\n");

	addr_size = sizeof(client_addr);

	unsigned long seq_no = 1;

	while(1)
	{
		bzero(buffer, 1024);
		recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
		if(strcmp(buffer, "Ready") == 0)
		{
			while(seq_no <= TIMEOUT)
			{
				// send data
				// printf("| %d | %s | %s \n", i, buffer, text);
				bzero(buffer, 1024);
				sprintf(buffer, "%lu \"Message\"", seq_no);
				// strcpy(buffer, "MESSAGE FROM SERVER");
				sendto(server_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
				printf(" >> Sent Data: %s\n", buffer);

				sprintf(text, "%lu \"Ack\"", seq_no);
				// Receive data
				bzero(buffer, 1024);
				recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
				if(strcmp(buffer, text) == 0)
				{
					printf(" >> Received from client: %s\n", buffer);
					seq_no = strtol(text, NULL, 10);	
					bzero(text, 1024);
					seq_no ++;
				}
				else
				{
					printf(" >! Error! Lost ack\n");
				}
				sleep(1);
			}
			bzero(buffer, 1024);
			strcpy(buffer, "\\QUIT");
			sendto(server_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
			printf(" >> Sent end of transmission: %s\n", buffer);
		}
		else if(strcmp(buffer, "\\QUIT") == 0)
		{
			printf(" >> Recieved disconnect from Client\n >> Closing...\n");
			break;
		}
	}

	return(0);
}
