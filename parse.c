#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void ls_proto(char args[100][50]);
void ls(char args[100][50], char init_dir[500]);
void cd(char args[100][50], char init_dir[500], char old_wd[500]);
void echo(char args[100][50]);
void pwd();

int execute_command(char *command, char init_dir[500], char old_wd[500])
{
    char args[100][50];
    char *token;
    token = strtok(command, " \t\n");
    int i = 0;
    while (token != NULL)
    {
        strcpy(args[i], token);
        token = strtok(NULL, " \t\n");
        i++;
    }
    strcpy(args[i], "\0");
    // printf("%s\n", args[0]);
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
    {
        exit(0);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        // printf("yes\n");
        pwd();
        return 1;
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args);
        return 2;
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        cd(args, init_dir, old_wd);
        return 3;
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        ls(args, init_dir);
        return 4;
    }
    else
    {
        printf("Error : Command not found\n");
        return 0;
    }
    // execvp(args[0], args[100][50]);
}

int parse(char *str, char commands[500][500], char background[500][500], char init_dir[500], char old_wd[500])
{
    char *token;
    token = strtok(str, ";");
    int i = 0;
    int j = 0;

    while (token != NULL)
    {
        // token = strtok(token, "&");
        // while (token != NULL)
        // {
        //     strcpy(background[j], token);
        //     token = strtok(NULL, "&");
        //     j++;
        // }
        // background[j][0] = '\0';

        strcpy(commands[i], token);
        // commands[i][strlen(token)] = '\0';
        token = strtok(NULL, ";");
        i++;
    }
    commands[i][0] = '\0';
    int flag = 0;
    int ret_val = 0;
    for (int j = 0; j < i; j++)
    {
        // printf("%s\n", commands[j]);
        ret_val = execute_command(commands[j], init_dir, old_wd);
        if (ret_val == 3)
        {
            flag = 1;
        }
    }
    return flag;
}
