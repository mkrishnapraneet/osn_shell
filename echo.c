#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void echo(char** args)
{
    int i = 1;
    while (args[i] != NULL)
    {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
}
