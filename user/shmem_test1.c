#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char *va = (char *)malloc(4096);
    if (va == 0)
    {
        printf("malloc failed\n");
        return 1;
    }

    int pid = fork();
    if (pid == -1)
    {
        printf("fork failed\n");
        return 1;
    }
    if (pid != 0)
    {
        if (map_shared_pages(getpid(), pid, (uint64)va, sizeof(va)) < 0)
        {
            printf("map_shared_pages failed\n");
            return 1;
        }

        strcpy(va, "Hello child\0");
        printf("parent says va contains: %s\n", va);

        wait(0);
    }
    else
    {
        sleep(20);
        printf("va contains: %s\n", va);
    }
    return 0;
}