#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char *va = (char *)malloc(100);
    if (va == 0)
    {
        printf("malloc failed\n");
        return 1;
    }

    printf("Parent allocated va: %p\n", va);

    int pid = getpid();

    int f = fork();
    if (f == -1)
    {
        printf("fork failed\n");
        return 1;
    }
    if (f != 0)
    {
        sleep(10);
        printf("parent says va contains: %s\n", va);

        wait(0);
    }
    else
    {
        uint64 p = map_shared_pages(pid, getpid(), (uint64)va, 100);
        if (p < 0)
        {
            printf("map_shared_pages failed\n");
            return 1;
        }
        strcpy((char *)p, "Hello daddy\0");

        sleep(20);

        unmap_shared_pages(getpid(), p, 100);
    }
    return 0;
}