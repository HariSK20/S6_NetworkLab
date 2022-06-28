#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

typedef sem_t Semaphore;
struct sockaddr_in server_addr;
int sock, n, quit_flag;
socklen_t addr_size;

void* get_message(void *vargp)
{
	char recv_buffer[1024];
	bzero(recv_buffer, 1024);
	int len;
	int *sock = (int *)vargp;
	while(quit_flag)
	{	
		if((len = recv(*sock, recv_buffer, 1024, 0)) != 0)
		{
			// printf("len = %d", len);
			recv_buffer[len > 0 ? len : 0] = '\0';
			if(strcmp(recv_buffer, "\0") != 0)
			{
				printf("\r\n >recieved message from client %s\n>>> ", recv_buffer);
				fflush(stdout);
				fflush(stdin);
				bzero(recv_buffer, 1024);
			}
		}
	}
	return(NULL);
}


int main()
{
	// IP and port to use
	char *ip = "127.0.0.2";
	char c;
	int port = 5566;
	char text[101], client_id[10];

	// To control print statements
	Semaphore mutex;
	sem_init(&mutex, 0, 1);
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


	// send data
	bzero(buffer, 1024);
	strcpy(buffer, "\\HELLO");
	send(sock, buffer, strlen(buffer), 0);
	printf(" >> Sent string: %s\n", buffer);

	// receive data
	bzero(buffer, 1024);
	recv(sock, buffer, 1024, 0);
	printf(" >> Got response: %s\n", buffer);
	strcpy(client_id, buffer);
	quit_flag = 1;
	pthread_t *message_recieve = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_create(message_recieve, NULL, (void *)get_message, (void *)&sock);
	printf(" > Enter text to send\n");
	while(1)
	{

		printf("\r>>> ");
		scanf("%[^\n]s", text);
		bzero(buffer, 1024);
		strcpy(buffer, client_id);
		if(strcmp(text, "quit") == 0)
		{
			// bzero(buffer, 1024);
			// strcpy(buffer, client_id);
			strcat(buffer, " : \\QUIT");
			send(sock, buffer, strlen(buffer), 0);			
			break;
		}
		else
		{
			// printf("not quit\n");
			// bzero(buffer, 1024);
			// strcpy(buffer, client_id);
			strcat(buffer, " : ");
			strcat(buffer, text);
			send(sock, buffer, strlen(buffer), 0);			
		}
		c = 'a';
		while(c!='\n')
			scanf("%c", &c);
		strcpy(text, "\0");

		// strcpy(client_id, buffer);

		// fflush(stdin);
	}
	quit_flag = 0;
	pthread_join(*message_recieve, NULL);
	// close connection
	close(sock);
	printf(" > Disconnected from server\n");

	return(0);
}
