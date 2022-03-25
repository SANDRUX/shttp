#include "http.h"

int createServer(const int port)
{
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockFd == -1)
    {
        fprintf(stderr, "Could not create socket\n");
        return -1;
    }

    struct sockaddr_in * addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    if (addr == NULL)
    {
        fprintf(stderr, "Could not allocate memory on the heap\n");
        return -1;
    }

    memset(addr, 0, sizeof(struct sockaddr_in));

    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;

    int status = bind(sockFd, (struct sockaddr *)addr, sizeof(struct sockaddr));

    free(addr);

    if (status == -1)
    {
        fprintf(stderr, "Could not bind the socket\n");
        return -1;
    }

    handleClients(sockFd, print_http_data);

    return 0;
}
    
int handleClients(const int sockFd, callback client_callback)
{
    int status = listen(sockFd, DEFAULT_BACKLOG);

    if (status == -1)
    {
        fprintf(stderr, "Could not make a listener socket\n");
        return -1;
    }

    for (;;)
    {
        int cfd = accept(sockFd, NULL, NULL);

        if (cfd == -1)
        {
            fprintf(stderr, "Coult not handle the client\n");
            return -1;
        }
        
        thread_data data;

        data.clientFd = cfd;
        data.handle = client_callback;

        pthread_t tId;

        if (pthread_create(&tId, NULL, thread_func, (void *)&data) != 0)
        {
            fprintf(stderr, "Could not create the thread with pid: %ld\n", tId);
            return -1;
        }
    }

    close(sockFd);

    return 0;
}

void * print_http_data(void * buffer)
{   
    parser_data * data = (parser_data *)buffer;

    for (size_t i = 0; i < data->size; i++)
    {
        printf("%c", data->buf[i]);
    }

    close(data->clientFd);
}

void * thread_func(void * data)
{
    thread_data tData = *((thread_data *)data);

    size_t numRead = 0;
    size_t size = 0;

    uint8_t * buf = (uint8_t *)malloc(MAXIMUM_CHUNK_SIZE);

    for (;;) 
    {
        uint8_t temp[MAXIMUM_CHUNK_SIZE];

        numRead = read(tData.clientFd, temp, MAXIMUM_CHUNK_SIZE);

        if (numRead > 0)
        {
            if (size != 0)
            {
                buf = (uint8_t *)realloc(buf, numRead + size);
            }
            memcpy(buf + size, temp, numRead);
            size += numRead;

            continue;
        }

        break;
    }

    parser_data parser_buf;
    parser_buf.buf = (uint8_t *)malloc(size);
    memcpy(parser_buf.buf, buf, size);

    parser_buf.clientFd = tData.clientFd;
    parser_buf.size = size;

    tData.handle((void *)&parser_buf);

    free(parser_buf.buf);
    free(buf);
}
