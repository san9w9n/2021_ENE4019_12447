CC = gcc

all : server client

server : server.c server.h header.h
	$(CC) -o server $^

client : client.c client.h client.h
	$(CC) -pthread -o client $^

clean :
	rm server client