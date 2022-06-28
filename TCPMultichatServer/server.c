#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

#define NO_OF_CLIENTS 128 

// need separate socket for each client

typedef struct client 
{
	unsigned int client_id;
	struct sockaddr_in client_addr;
	int client_sock;
	int disconnected;
	char buffer[1024];
}client;

// To handle multiple connections
unsigned int client_count = 0, disconnect_count = 0;
client clients[NO_OF_CLIENTS];


void *client_process(void *vargp)
{
	char *temp;
	client *self = (client *)vargp;
	while(1)
	{
		// Accept an incoming connection

		// Receive data
		bzero(self->buffer, 1024);
		recv(self->client_sock, self->buffer, 1024, 0);
		if(strcmp(self->buffer, "") == 0)
			break;
		printf(" >> Received from client %u: \"%s\"\n", self->client_id, self->buffer);

		// substring search
		temp = strstr(self->buffer, "\\QUIT");
		if(temp != NULL)
			break;
		// substring search
		temp = strstr(self->buffer, "\\");
		if(temp == NULL)
			for(int i = 0; i < NO_OF_CLIENTS; i++)
				if(clients[i].client_id != self->client_id && clients[i].disconnected == 0)
				{		
					if(send(clients[i].client_sock, self->buffer, strlen(self->buffer), 0) < 0)
					{
						printf(" !> Unable to send \"%s\" to id %u", self->buffer, clients[i].client_id);
					}
					else
						printf(" >> Sent response to %u: \"%s\"\n", clients[i].client_id, self->buffer);
				}
		// close the connection
	}
	close(self->client_sock);
	self->disconnected = 1;
	printf(" >> Client Disconnected\n");
	disconnect_count++;
	printf(" No of clients %u, no disconnected %u\n", client_count, disconnect_count);
	return(NULL);
}

int main()
{
	// IP and port to use
	char *ip = "127.0.0.2", *temp;
	int port = 5566;
	pthread_t client_threads[NO_OF_CLIENTS];
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
		perror(" !> Unable to create socket ");
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
		perror(" !> Unable to bind socket ");
		exit(-1);
	}
	printf(" > Socket Binding complete!\n");

	for(int i =0; i < NO_OF_CLIENTS; i++)
	{
		clients[i].client_id = NO_OF_CLIENTS +3;
		clients[i].disconnected = 1;
	}

	while(1)
	{
		// Start listening for incomming connections
		listen(server_sock, 5);
		// printf(" > Listening!\n");

		if(client_count < NO_OF_CLIENTS - 1)
		{		
			addr_size = sizeof(client_addr);
			client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
			printf(" >> Client Connected!\n");
			// send client id
			bzero(buffer, 1024);
			sprintf(buffer, "%u", client_count);
			send(client_sock, buffer, strlen(buffer), 0);
			clients[client_count].client_id = client_count;
			clients[client_count].client_addr = client_addr;
			clients[client_count].client_sock = client_sock;
			clients[client_count].disconnected = 0;
			bzero(clients[client_count].buffer, 1024);
			pthread_create(&client_threads[client_count], NULL, client_process, (void*)&clients[client_count]);				
			client_count++;
		}

		if(disconnect_count == client_count)
			break;		
	}

	for( int i = 0; i < client_count; i++)
		pthread_join(client_threads[i]	, NULL);

	return(0);
}
