#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include"proxy_header_S1934.h"


pthread_mutex_t mutex;
// need separate socket for each client

void *clientProcess(void *vargp)
{
	char buffer[BUFFER_SIZE], *temp;
	int *sock_pair = (int *)vargp;
	while(1)
	{
		// Receive data
		bzero(buffer, BUFFER_SIZE);
		recv(sock_pair[CLIENT], buffer, BUFFER_SIZE, 0);
		if(strcmp(buffer, "") == 0)
			break;
		printf(" >> Received from client : \"%s\" \n",buffer);

		if(send(sock_pair[SERVER], buffer, strlen(buffer), 0) < 0)
		{
			printf(" !> Unable to send \"%s\" to Server\n", buffer);
		}
		else
			printf(" >> Sent response to server \"%s\" \n", buffer);

		// substring search
		temp = strstr(buffer, "\\QUIT");
		if(temp != NULL)
			break;
	}
	// close the connection
	close(sock_pair[CLIENT]);
	printf(" > Disconnected from Client\n");
	return(NULL);
}

void *serverProcess(void *vargp)
{
	char buffer[BUFFER_SIZE], *temp;
	int *sock_pair = (int *)vargp;
	while(1)
	{
		// Receive data
		bzero(buffer, BUFFER_SIZE);
		recv(sock_pair[SERVER], buffer, BUFFER_SIZE, 0);
		if(strcmp(buffer, "") == 0)
			break;
		printf(" >> Received from Server : \"%s\" \n",buffer);

		if(send(sock_pair[CLIENT], buffer, strlen(buffer), 0) < 0)
		{
			printf(" !> Unable to send \"%s\" to Client\n", buffer);
		}
		else
			printf(" >> Sent response to Client \"%s\" \n", buffer);
		// substring search

		temp = strstr(buffer, "\\QUIT");
		if(temp != NULL)
			break;
	}
	// close the connection
	close(sock_pair[SERVER]);
	printf(" > Disconnected from server\n");
	return(NULL);
}

int main()
{
	// IP and port to use
	char *ip = "127.0.0.2", *temp;
	int port_client = CLIENT_PORT;
	int port_server = SERVER_PORT;
	char buffer[BUFFER_SIZE];

	pthread_t client_thread, server_thread;
	// The required data structures for handling the addresses
	struct sockaddr_in server_addr, client_addr;

	// Socket file descriptors
	int sock_pair[2];

	socklen_t addr_size;
	int n;
	
	// The buffer using which we pass data

	// memset(buffer, 0, 1024);

	// create server socket
	sock_pair[SERVER] = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_pair[SERVER] < 0)
	{
		perror(" !> Unable to create socket ");
		exit(-1);
	}
	printf(" > Socket Created!\n");

	// Setting up the server address
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port_server;
	server_addr.sin_addr.s_addr = inet_addr(ip);

	// Create a Client socket for IPv4 Address Family, and a Bidirectional TCP stream
	sock_pair[CLIENT] = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_pair[CLIENT] < 0)
	{
		perror(" !> Unable to create socket ");
		exit(-1);
	}
	printf(" > Socket Created!\n");

	// Setting up the client address
	memset(&client_addr, '\0', sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = port_client;
	client_addr.sin_addr.s_addr = inet_addr(ip);

	// Binding the address to socket
	n = bind(sock_pair[CLIENT], (struct sockaddr*)&client_addr, sizeof(client_addr));
	if(n < 0)
	{
		perror(" !> Unable to bind client socket ");
		exit(-1);
	}
	printf(" > Client Socket Binding complete!\n");


	// connect server
	n = connect(sock_pair[SERVER], (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(n < 0)
	{
		perror(" !> Unable to connect to server!");
		exit(1);
	}
	printf(" > Connected to server\n");

	// send client id
	bzero(buffer, 1024);
	sprintf(buffer, "%d proxy connected", SERVER);
	send(sock_pair[SERVER], buffer, sizeof(buffer), 0);
	pthread_create(&server_thread, NULL, serverProcess, (void *)sock_pair);

	pthread_mutex_lock(&mutex);
	// connect client
	listen(sock_pair[CLIENT], 5);
	addr_size = sizeof(client_addr);
	sock_pair[CLIENT] = accept(sock_pair[CLIENT], (struct sockaddr*)&client_addr, &addr_size);
	printf(" >> Client Connected!\n");
	// send client id
	bzero(buffer, 1024);
	sprintf(buffer, "%d client connected", CLIENT);
	send(sock_pair[CLIENT], buffer, sizeof(buffer), 0);
	pthread_create(&client_thread, NULL, clientProcess, (void *)sock_pair);

	pthread_mutex_unlock(&mutex);

	// for( int i = 0; i < client_count; i++)
	pthread_join(client_thread, NULL);
	pthread_join(server_thread, NULL);

	return(0);
}
