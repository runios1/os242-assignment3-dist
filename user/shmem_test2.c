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
        printf("parent says va contains: %s\n", va);

        wait(0);
    }
    else
    {
        printf("Child memory before mapping: %d\n", sbrk(0));

        uint64 p = map_shared_pages(pid, (uint64)va, 4096);

        printf("Child memory after mapping: %d\n", sbrk(0));

        if (p < 0)
        {
            printf("map_shared_pages failed\n");
            return 1;
        }
        strcpy((char *)p, "Hello daddy");

        unmap_shared_pages(p, 100);

        printf("Child memory after unmapping: %d\n", sbrk(0));

        void *pointer = malloc(100000);

        printf("Child memory after malloc: %d\n", sbrk(0));

        free(pointer);
    }
    return 0;
}