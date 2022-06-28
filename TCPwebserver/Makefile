all: server client proxy

server: server.c 
	gcc server.c -pthread -o server.out

client: client.c
	gcc client.c -pthread -o client.out

proxy: proxy.c 
	gcc proxy.c -pthread -o proxy.out
