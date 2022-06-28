#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<time.h>

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

	addr_size = sizeof(client_addr);

	time_t t;
	struct tm lt;
	while(1)
	{
		// Receive data
		bzero(buffer, 1024);
		recvfrom(server_sock, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);

		bzero(buffer, 1024);
		t = time(NULL);
		lt = *localtime(&t);
		// if (strftime(buffer, sizeof(buffer), format, &lt) == 0) {
  //               printf("strftime(3): cannot format supplied date/time into buffer of size %u using: '%s'\n", sizeof(res), format);
  //               return 1;
  //       }
		sprintf(buffer, "%d/%d/%d %d:%d:%d", lt.tm_mday, lt.tm_mon, lt.tm_year+1900, lt.tm_hour, lt.tm_min, lt.tm_sec);
		// strcpy(buffer, "MESSAGE FROM SERVER");
		sendto(server_sock, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
		printf(" >> Sent response: %s\n", buffer);
	}

	return(0);
}
