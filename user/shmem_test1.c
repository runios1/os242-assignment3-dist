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
        map_shared_pages(getpid(), pid, (uint64)va, 4096);
        strcpy(va, "Hello child");
        wait(0);
    }
    else
    {
        sleep(20);
        printf(va);
    }
    return 0;
}