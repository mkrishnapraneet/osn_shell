#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void cd(char args[100][50], char init_dir[500], char old_wd[500])
{
    if (args[2][0] != '\0')
    {
        printf("Error: Too many arguments\n");
    }
    else if (args[1][0] == '\0' || strcmp(args[1], "~") == 0)
    {
        int val = chdir(init_dir);
        if (val == -1)
        {
            perror("chdir() error");
            return;
        }
    }
    else if (args[1][0] == '~' && args[1][1] == '/')
    {
        int val = chdir(init_dir);
        if (val == -1)
        {
            perror("chdir() error");
            return;
        }
        args[1][0] = '.';
        val = chdir(args[1]);
        if (val == -1)
        {
            perror("chdir() error");
            return;
        }
    }
    else if (args[1][0] == '-')
    {
        int val = chdir(old_wd);
        if (val == -1)
        {
            perror("chdir() error");
            return;
        }
    }
    else
    {
        int val = chdir(args[1]);
        if (val == -1)
        {
            perror("chdir() error");
            return;
        }
    }
}