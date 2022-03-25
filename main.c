#include "http.h"

int main()
{   
    int status = createServer(8000);

    if (status != 0)
    {
        fprintf(stderr, "Error occured while running the server!");

        return -1;
    }

    return 0;
}
