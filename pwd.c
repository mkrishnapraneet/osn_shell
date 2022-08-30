#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void pwd()
{
    char cwd[500];
    getcwd(cwd, sizeof(cwd));
    if (cwd == NULL)
    {
        perror("getcwd() error : ");
        return;
    }
    printf("%s\n", cwd);
}