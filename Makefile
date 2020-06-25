CC = g++ 
CFLAGS  = -Wall -ggdb -pthread

all: server client

server: Server.o Semaphore.o
	$(CC) $(CFLAGS) -o server Server.o Semaphore.o

client: Client.o Semaphore.o
	$(CC) $(CFLAGS) -o client Client.o Semaphore.o

Server.o: Server.cpp mesg.h
	$(CC) $(CFLAGS) -O -c Server.cpp mesg.h

Client.o: Client.cpp mesg.h
	$(CC) $(CFLAGS) -O -c Client.cpp mesg.h 

Semaphore.o: Semaphore.cpp 
	$(CC) $(CFLAGS) -O -c Semaphore.cpp 
