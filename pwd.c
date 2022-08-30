#include <stdio.h>
#include <unistd.h>

void pwd()
{
    char cwd[500];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}