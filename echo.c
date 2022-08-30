#include <stdio.h>
#include <unistd.h>
#include <string.h>

void echo(char args[100][50])
{
    int i = 1;
    while (args[i][0] != '\0')
    {
        printf("%s ", args[i]);
        i++;
    }
    printf("\n");
}
