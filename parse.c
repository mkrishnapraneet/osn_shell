#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void ls_proto(char args[100][50]);
void ls(char** args, char init_dir[500]);
void cd(char** args, char init_dir[500], char old_wd[500]);
void echo(char** args);
void pwd();

void free_mem(char** args, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(args[i]);
    }
    free(args);
}

int execute_command(char *command, char init_dir[500], char old_wd[500])
{
    // char args[100][50];
    // declare args as 2D array using malloc
    char **args = (char **)calloc(100, sizeof(char *));

    for (int c = 0; c < 100; c++)
    {
        args[c] = (char *)calloc(50, sizeof(char));
    }
    char *token;
    token = strtok(command, " \t\n");
    int i = 0;
    while (token != NULL)
    {
        strcpy(args[i], token);
        token = strtok(NULL, " \t\n");
        i++;
    }

    // strcpy(args[i], "\0");        ////////////// important

    args[i] = NULL;

    // printf("%s\n", args[0]);
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0)
    {
        free_mem(args, 100);
        exit(0);
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        // printf("yes\n");
        pwd();
        free_mem(args, 100);
        return 1;
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args);
        free_mem(args, 100);
        return 2;
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        cd(args, init_dir, old_wd);
        free_mem(args, 100);
        return 3;
    }
    else if (strcmp(args[0], "ls") == 0)
    {
        ls(args, init_dir);
        free_mem(args, 100);
        return 4;
    }
    else
    {
        printf("Error : Command not found\n");
        free_mem(args, 100);
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
