#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_BACKLOG 128
#define MAXIMUM_CHUNK_SIZE 4096

typedef void* (*callback)(void *);

typedef struct
{
    int clientFd;
    callback handle;
}thread_data;

int createServer(const int);
int handleClients(const int, callback);
void * thread_func(void *);