#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid = getpid();

    char *va = (char *)malloc(4096);
    if (va == 0)
    {
        printf("malloc failed\n");
        return 1;
    }

    int f = fork();
    if (f == -1)
    {
        printf("fork failed\n");
        return 1;
    }
    if (f != 0)
    {

        // printf("Parent allocated va: %p\n", va);
        strcpy(va, "Hello child\0");
        // printf("parent says va contains: %s\n", va);

        wait(0);
    }
    else
    {
        uint64 p = map_shared_pages(pid, getpid(), (uint64)va, 4096);
        if (p == 0xFFFFFFFFFFFFFFFF)
        {
            printf("map_shared_pages failed\n");
            return 1;
        }
        printf("Child mapped p: %p\n", (void *)p);
        // sleep(20);

        printf("p contains: %s\n", (char *)p);
    }
    return 0;
}