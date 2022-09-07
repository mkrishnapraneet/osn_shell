#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

struct parsed_comm
{
    char command[500];
    int isBackgroundProcess;
};

void discover(char ** args, char init_dir[500]);
void pinfo(char **args, char init_dir[500]);
void ls_proto(char args[100][50]);
void ls(char **args, char init_dir[500]);
void cd(char **args, char init_dir[500], char old_wd[500]);
void echo(char **args);
void pwd();

void free_mem(char **args, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(args[i]);
    }
    free(args);
}

int execute_command(struct parsed_comm parsed_command, char init_dir[500], char old_wd[500], char history[20][500])
{
    // char args[100][50];
    // declare args as 2D array using malloc
    char **args = (char **)calloc(100, sizeof(char *));

    int isBackgroundProcess = parsed_command.isBackgroundProcess;

    for (int c = 0; c < 100; c++)
    {
        args[c] = (char *)calloc(50, sizeof(char));
        // args[c] = NULL;
    }

    char *token;
    token = strtok(parsed_command.command, " \t\n");
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
        // return 0;
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
    else if (strcmp(args[0], "pinfo") == 0)
    {
        pinfo(args, init_dir);
        free_mem(args, 100);
        return 5;
    }
    else if (strcmp(args[0], "discover") == 0)
    {
        discover(args, init_dir);
        free_mem(args, 100);
        return 6;
    }
    else if (strcmp(args[0], "history") == 0)
    {
        // discover(args, init_dir);
        for (int k = 0; k < 10; k++)
        {
            printf("%s\n", history[k]);
        }
        free_mem(args, 100);
        return 7;
    }
    else
    {
        // printf("Error : Command not found\n");
        pid_t pid;
        pid = fork();
        if (pid == 0)
        {
            int val_ret = execvp(args[0], args);
            if (val_ret == -1)
            {
                perror("execvp() error");
                free_mem(args, 100);
                return -1;
            }
        }
        else if (pid > 0)
        {
            int status;
            if (isBackgroundProcess == 0)
            {
                int wait_val = waitpid(pid, &status, 0);
                if (wait_val == -1)
                {
                    perror("waitpid() error");
                    free_mem(args, 100);
                    return -1;
                }
            }
            else
            {
                printf("Process with PID %d started in the background\n", pid);
            }
            // waitpid(pid, &status, 0);
            free_mem(args, 100);
            return 9;
        }
        else
        {
            perror("fork() error");
            free_mem(args, 100);
            return -1;
        }
    }
    // execvp(args[0], args[100][50]);
}

int parse(char *str, char commands[500][500], struct parsed_comm parsed_commands[500], char background[500][500], char init_dir[500], char old_wd[500], char history[20][500])
{
    char *token;
    token = strtok(str, ";\n");
    int i = 0;
    int e = 0;

    for (int c = 0; c < 500; c++)
    {
        background[c][0] = '\0';
    }

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
        token = strtok(NULL, ";\n");
        i++;
    }
    commands[i][0] = '\0';

    // struct parsed_comm parsed_commands[500];

    for (int c = 0; c < 500; c++)
    {
        parsed_commands[c].isBackgroundProcess = 0;
    }

    int ind = 0;
    for (int c = 0; c < i; c++)
    {
        int num_ands = 0;
        for (int w = 0; w < strlen(commands[c]); w++)
        {
            if (commands[c][w] == '&')
            {
                num_ands++;
            }
        }
        // parse commands[c] for & and store in parsed_commands[c].command
        char *n_token;
        // if (commands[c][0] == '\0')
        // {
        //     continue;
        // }
        n_token = strtok(commands[c], "&\n");
        int g = 0;
        while (n_token != NULL)
        {
            strcpy(parsed_commands[ind].command, n_token);
            n_token = strtok(NULL, "&\n");
            ind++;
            g++;
        }
        if (g > 1)
        {

            for (int z = ind - g; z < ind - 1; z++)
            {
                parsed_commands[z].isBackgroundProcess = 1;
            }
            if (num_ands == g)
            {
                // printf("Error : Invalid command\n");
                // return -1;
                parsed_commands[ind - 1].isBackgroundProcess = 1;
            }
        }
        else if (g == 1 && num_ands == 1)
        {
            parsed_commands[ind - 1].isBackgroundProcess = 1;
        }
        // else if (g == 0)
        // {
        //     printf("Error : Invalid command\n");
        //     return -1;
        //     // continue;
        // }
    }

    int flag = 0;
    int ret_val = 0;
    for (int j = 0; j < ind; j++)
    {
        // printf("%s\n", commands[j]);
        ret_val = execute_command(parsed_commands[j], init_dir, old_wd, history);
        if (ret_val == 3)
        {
            flag = 1;
        }
    }
    return flag;
}
