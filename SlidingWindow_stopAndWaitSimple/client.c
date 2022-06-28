#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>

#define TIMEOUT 10

int main()
{
	// IP and port to use
	char *ip = "127.0.0.2";
	int port = 5566;

	// The required data structures for handling the addresses
	struct sockaddr_in server_addr;
	int sock, n;
	socklen_t addr_size;
	char buffer[1024];

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

	unsigned long seq_no = 1;
	// send data
	bzero(buffer, 1024);
	sprintf(buffer, "Ready");
	sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf(" >> Sent string: %s\n", buffer);

	printf(" >> Connected to server\n");
	while(seq_no <= TIMEOUT)
	{
		// receive data
		bzero(buffer, 1024);
		recvfrom(sock, buffer, 1024, 0, (struct sockaddr*)&server_addr, &addr_size);
		printf(" >> Recieved Transmission: %s\n", buffer);

		if(strcmp(buffer, "\\QUIT") == 0)
		{
			printf(" >> Recieved end of transmission\n");
			break;
		}

		sleep(1);
		seq_no = strtol(buffer, NULL, 10);
		bzero(buffer, 1024);
		sprintf(buffer, "%lu \"Ack\"", seq_no);
		sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
	}

	printf(" >> Closing connection\n");
	bzero(buffer, 1024);
	strcpy(buffer, "\\QUIT");
	sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

	return(0);
}
