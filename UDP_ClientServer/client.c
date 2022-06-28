#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>


int main()
{
	// IP and port to use
	char *ip = "127.0.0.2"	;
	int port = 5566;

	// The required data structures for handling the addresses
	struct sockaddr_in server_addr;
	int sock, n;
	socklen_t addr_size;
	char buffer[1024], text[1024], c;

	// Create a socket for IPv4 Address Family, and a Bidirectional UDP DGRAM
	sock = socket(AF_INET, SOCK_DGRAM, 0);
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

	// send data
	// bzero(buffer, 1024);
	// strcpy(buffer, "THIS IS A CLIENT");
	// sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
	// printf(" >> Sent string: %s\n", buffer);


	while(1)
	{
		printf(" > Enter text to send\n");
		printf("\r >>> ");
		scanf("%[^\n]s", text);
		bzero(buffer, 1024);
		if(strcmp(text, "quit") == 0)
		{
			strcpy(buffer, "\\QUIT");
			sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
			break;
		}
		else
		{
			strcpy(buffer, text);
			sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		}
		c = 'a';
		while(c!='\n')
			scanf("%c", &c);
		strcpy(text, "\0");
		// fflush(stdin);

		// receive data
		bzero(buffer, 1024);
		recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
		printf(" >> Got response: %s\n", buffer);
	}


	return(0);
}
