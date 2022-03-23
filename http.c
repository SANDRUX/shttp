#include "http.h"

int createServer(const int port)
{
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockFd == -1)
    {
        fprintf(stderr, "Could not create socket");
        return -1;
    }

    struct sockaddr_in * addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    if (addr == NULL)
    {
        fprintf(stderr, "Could not allocate memory on the heap");
        return -1;
    }

    memset(addr, 0, sizeof(struct sockaddr_in));

    int port;
    fprintf(stdout, "Enter the port number: ");
    fscanf(stdin, "%d", &port);

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;

    int status = bind(sockFd, (struct sockaddr *)addr, sizeof(struct sockaddr));

    free(addr);

    if (status == -1)
    {
        fprintf(stderr, "Could not bind the socket");
        return -1;
    }
}
    
int handleClients(const int sockFd, callback client_callback)
{
    int status = listen(sockFd, DEFAULT_BACKLOG);

    if (status == -1)
    {
        fprintf(stderr, "Could not make a listener socket");
        return -1;
    }

    for (;;)
    {
        int cfd = accept(sockFd, NULL, NULL);

        if (cfd == -1)
        {
            fprintf(stderr, "Coult not handle the client");
            return -1;
        }
        
        thread_data data;

        data.clientFd = cfd;
        data.handle = client_callback;

        pthread_t pId;

        if (pthread_create(&pId, NULL, thread_func, (void *)&data) != 0)
        {
            fprintf(stderr, "Could not create the thread with pid: %d", pId);
            return -1;
        }
    }

    close(sockFd);
}

void * thread_func(void * data)
{
    thread_data tData = *((thread_data *)data);

    uint8_t * buf = (uint8_t *)malloc(MAXIMUM_CHUNK_SIZE);

    size_t numRead = 0;
    size_t alloc_size = 0;

    for (;;)
    {
        uint8_t temp[MAXIMUM_CHUNK_SIZE];

        numRead = read(tData.clientFd, temp, MAXIMUM_CHUNK_SIZE);

        if (numRead > 0)
        {
            if (alloc_size != 0)
            {
                buf = (uint8_t *)realloc(buf, MAXIMUM_CHUNK_SIZE + alloc_size);
            }
            memcpy(buf + alloc_size, temp, numRead);
            alloc_size += numRead;

            continue;
        }

        break;
    }

    tData.handle(buf);

    free(buf);
}