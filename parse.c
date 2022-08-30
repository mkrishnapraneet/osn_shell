#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void cd(char args[100][50], char init_dir[500]);
void echo(char args[100][50]);
void pwd();

void execute_command(char *command, char init_dir[500])
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
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        echo(args);
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        cd(args, init_dir);
    }
    else
    {
        printf("Error : Command not found\n");
    }
    // execvp(args[0], args[100][50]);
}

void parse(char *str, char commands[500][500], char background[500][500], char init_dir[500])
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

    for (int j = 0; j < i; j++)
    {
        // printf("%s\n", commands[j]);
        execute_command(commands[j], init_dir);
    }
}
