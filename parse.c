#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

extern int active_pid;
extern int bg_counter;
extern int parent_pid;

struct inp_out
{
    int isInputFile;
    int isOutputFile;
};

struct redir_flags
{
    int in;
    int out;
    int append;
};

struct parsed_comm
{
    char command[500];
    int isBackgroundProcess;
};

struct background_process
{
    int pid;
    char name[500];
    int s_no;
    // int status;
};

extern struct background_process back_proc[500];

int input_fd = 0;
int output_fd = 1;

void fg(char **args, struct background_process back_proc[500]);
void bg(char **args, struct background_process back_proc[500]);
void sig(char **args, struct background_process back_proc[500]);
void jobs(char **args, struct background_process back_proc[500]);
void discover(char **args, char init_dir[500]);
void pinfo(char **args, char init_dir[500]);
void ls_proto(char args[100][50]);
void ls(char **args, char init_dir[500]);
void cd(char **args, char init_dir[500], char old_wd[500]);
void echo(char **args);
void pwd();

void CTRL_C_handler(int sign);
void CTRL_Z_handler(int sign);

// void CTRL_C_handler(int sign)
// {
//     // printf("detected ctrc c\n");
//     // if (getpid() == parent_pid)
//     // {
//     //     // printf("tis\n");
//     //     if (active_pid != -1)
//     //     {
//     //         // check if it is a background process
//     //         // int i = 0;
//     //         // while (back_proc[i].pid != active_pid && i < 500)
//     //         // {
//     //         //     i++;
//     //         // }
//     //         // if (i >= 500) // if it is not a background process
//     //         // {
//     //         printf("killing %d", active_pid);
//     //         // kill(active_pid, SIGINT);
//     //         // }
//     //     }
//     //     // signal(SIGINT, CTRL_C);
//     //     // printf
//     // }

//     if (getpid() != parent_pid)
//         return;

//     if (active_pid != -1)
//     {
//         kill(active_pid, SIGINT);
//     }
//     return;
//     // signal(SIGINT, CTRL_C_handler);
// }

// void CTRL_Z_handler(int signal)
// {
//     // printf("\ndetected ctrl z\n");
//     if (getpid() != parent_pid)
//         return;

//     if (active_pid != -1)
//     {
//         kill(active_pid, SIGTSTP);
//         // add it to background processes
//         int i = 0;
//         while (back_proc[i].pid != -1 && i < 500)
//         {
//             i++;
//         }
//         if (i >= 500)
//         {
//             printf("Error: Too many background processes\n");
//             return;
//         }
//         back_proc[i].pid = active_pid;
//         back_proc[i].s_no = bg_counter;
//         bg_counter++;
//         // get the name of the process
//         char buffer[500];
//         sprintf(buffer, "/proc/%d/cmdline", active_pid);
//         FILE *fd = fopen(buffer, "r");
//         if (fd == NULL)
//         {
//             printf("Error: %s\n", strerror(errno));
//             return;
//         }
//         char *pname;
//         long size = 0;
//         getline(&pname, &size, fd);
//         strcpy(back_proc[i].name, pname);
//     }
// }

void free_mem(char **args, int size)
{
    for (int i = 0; i < size; i++)
    {
        free(args[i]);
    }
    free(args);
}

struct inp_out redirect(char **args)
{
    int i = 0;
    struct inp_out io;
    io.isInputFile = 0;
    io.isOutputFile = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], "<") == 0)
        {
            input_fd = open(args[i + 1], O_RDONLY);
            // printf("came here");
            if (input_fd < 0)
            {
                perror("Error opening input file");
                return io;
            }
            io.isInputFile = 1;
            // remove the < and the file name from the args take care of null pointer
            int j = i + 2;
            if (args[j] == NULL)
            {
                args[i] = NULL;
            }
            else
            {
                while (args[j] != NULL)
                {
                    free(args[j - 2]);
                    args[j - 2] = args[j];

                    args[j] = NULL;
                    j++;
                }
                args[j - 2] = NULL;
            }
        }
        else if (strcmp(args[i], ">") == 0)
        {
            output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0)
            {
                perror("Error opening output file");
                return io;
            }
            io.isOutputFile = 1;
            // remove the > and the file name from the args
            int j = i + 2;
            if (args[j] == NULL)
            {
                args[i] = NULL;
            }
            else
            {
                while (args[j] != NULL)
                {
                    free(args[j - 2]);
                    args[j - 2] = args[j];

                    args[j] = NULL;
                    j++;
                }
                args[j - 2] = NULL;
            }
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (output_fd < 0)
            {
                perror("Error opening output file");
                return io;
            }
            io.isOutputFile = 1;
            // remove the >> and the file name from the args
            int j = i + 2;
            if (args[j] == NULL)
            {
                args[i] = NULL;
            }
            else
            {
                while (args[j] != NULL)
                {
                    free(args[j - 2]);
                    args[j - 2] = args[j];

                    args[j] = NULL;
                    j++;
                }
                args[j - 2] = NULL;
            }
        }
        else
        {
            i++;
        }
    }
    // printf("yes");
    return io;
}

int act_execute(char **args, char init_dir[500], char old_wd[500], char history[20][500], struct background_process back_proc[500], int isBackgroundProcess, struct redir_flags redir_flags, int sav_stdin, int sav_stdout)
{
    if (redir_flags.in + redir_flags.out + redir_flags.append > 2)
    {
        printf("Error: Multiple redirections not allowed");
        return 1;
    }
    input_fd = 0;
    output_fd = 1;

    struct inp_out io = redirect(args);

    if (io.isInputFile)
    {
        dup2(input_fd, 0);
        close(input_fd);
    }

    if (io.isOutputFile)
    {
        dup2(output_fd, 1);
        close(output_fd);
    }

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
    else if (strcmp(args[0], "jobs") == 0)
    {
        jobs(args, back_proc);
        free_mem(args, 100);
        return 8;
    }
    else if (strcmp(args[0], "sig") == 0)
    {
        sig(args, back_proc);
        free_mem(args, 100);
        return 10;
    }
    else if (strcmp(args[0], "bg") == 0)
    {
        bg(args, back_proc);
        free_mem(args, 100);
        return 11;
    }
    else if (strcmp(args[0], "fg") == 0)
    {
        fg(args, back_proc);
        free_mem(args, 100);
        return 12;
    }
    else
    {
        // printf("Error : Command not found\n");
        pid_t pid;
        pid = fork();
        // active_pid = pid;
        if (pid == 0)
        {
            int val_ret = execvp(args[0], args);

            if (val_ret == -1)
            {
                perror("execvp() error");
                free_mem(args, 100);
                return -1;
            }

            // signal(SIGINT, CTRL_C_handler);
            // signal(SIGTSTP, CTRL_Z_handler);
        }
        else if (pid > 0)
        {
            int status;
            // signal(SIGINT, SIG_IGN);
            // signal(SIGTSTP, SIG_IGN);
            if (isBackgroundProcess == 0)
            {
                active_pid = pid;
                int wait_val = waitpid(pid, &status, WUNTRACED);
                if (wait_val == -1)
                {
                    perror("waitpid() error");
                    free_mem(args, 100);
                    return -1;
                }
            }
            else
            {
                // kill(pid, SIGTTIN);
                // kill(pid, SIGCONT);

                active_pid = -1;
                printf("Process with PID %d started in the background\n", pid);
                // iterate to found the first empty slot in the array
                int i = 0;
                while (back_proc[i].pid != -1)
                {
                    i++;
                }
                back_proc[i].pid = pid;
                strcpy(back_proc[i].name, args[0]);
                back_proc[i].s_no = bg_counter % 500;
                bg_counter++;
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
    exit(0);
}

void piping(char **args, char init_dir[500], char old_wd[500], char history[20][500], struct background_process back_proc[500], int isBackgroundProcess, struct redir_flags redir_flags, int sav_stdin, int sav_stdout)
{
    // concatenate all the args into a single string with spaces
    char full_command[500];
    strcpy(full_command, "");
    int i = 0;
    while (args[i] != NULL)
    {
        strcat(full_command, args[i]);
        strcat(full_command, " ");
        i++;
    }

    int j = 0;
    int input_fd = 0;
    char command[50][500];
    i = 0;
    char *token = strtok(full_command, "|");
    while (token != NULL)
    {
        strcpy(command[i], token);
        command[i++][strlen(token)] = '\0';
        token = strtok(NULL, "|");
    }

    command[i][0] = '\0';

    for (i = 0; command[i][0] != '\0'; ++i)
    {
        int pipefd[2];
        pipe(pipefd); // create a pipe connection

        pid_t pid = fork();

        if (pid == 0)
        {
            dup2(input_fd, 0); // change the input according to the old one

            if (command[i + 1][0] != '\0')
            {
                dup2(pipefd[1], 1); // change the output according to the pipe
            }

            close(pipefd[0]); // close the reading end of the pipe

            char **command_args = (char **)calloc(100, sizeof(char *));
            for (int i = 0; i < 100; i++)
            {
                command_args[i] = (char *)calloc(100, sizeof(char));
            }

            int j = 0;

            char *token2 = strtok(command[i], " \t\n");

            while (token2 != NULL)
            {
                strcpy(command_args[j], token2);
                command_args[j++][strlen(token2)] = '\0';
                token2 = strtok(NULL, " \t\n");
            }

            // command_pipe[j][0] = '\0';
            command_args[j] = NULL;

            // printf("%s: pop\n",command_pipe[0] );
            act_execute(command_args, init_dir, old_wd, history, back_proc, isBackgroundProcess, redir_flags, sav_stdin, sav_stdout);
            dup2(sav_stdin, 0);
            dup2(sav_stdout, 1);
            // printf("done\n" );
            // free_mem(command_pipe, 500);
            exit(2);
        }
        else
        {
            // printf("::%d\n",i );
            wait(NULL);
            close(pipefd[1]);
            input_fd = pipefd[0];
        }
    }
}

int execute_command(struct parsed_comm parsed_command, char init_dir[500], char old_wd[500], char history[20][500], struct background_process back_proc[500])
{
    // char args[100][50];
    // declare args as 2D array using malloc
    char **args = (char **)calloc(100, sizeof(char *));

    int isBackgroundProcess = parsed_command.isBackgroundProcess;

    for (int c = 0; c < 100; c++)
    {
        args[c] = (char *)calloc(100, sizeof(char));
        // args[c] = NULL;
    }

    int sav_stdin = dup(0);
    int sav_stdout = dup(1);

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

    // check if args has '|' or '<' or '>' or '>>'
    int pipe_flag = 0;

    struct redir_flags redir_flags;
    redir_flags.in = 0;
    redir_flags.out = 0;
    redir_flags.append = 0;

    for (int j = 0; j < i; j++)
    {
        if (strcmp(args[j], "|") == 0)
        {
            pipe_flag = 1;
        }
        if (strcmp(args[j], "<") == 0)
        {
            redir_flags.in = 1;
        }
        if (strcmp(args[j], ">") == 0)
        {
            redir_flags.out = 1;
        }
        if (strcmp(args[j], ">>") == 0)
        {
            redir_flags.append = 1;
        }
    }

    if (pipe_flag == 1)
    {
        piping(args, init_dir, old_wd, history, back_proc, isBackgroundProcess, redir_flags, sav_stdin, sav_stdout);
    }
    else
    {
        int ret_val = act_execute(args, init_dir, old_wd, history, back_proc, isBackgroundProcess, redir_flags, sav_stdin, sav_stdout);
        dup2(sav_stdin, 0);
        dup2(sav_stdout, 1);
        return ret_val;
    }
    // execvp(args[0], args[100][50]);
}

int parse(char *str, char commands[500][500], struct parsed_comm parsed_commands[500], char background[500][500], char init_dir[500], char old_wd[500], char history[20][500], struct background_process back_proc[500])
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
        ret_val = execute_command(parsed_commands[j], init_dir, old_wd, history, back_proc);
        if (ret_val == 3)
        {
            flag = 1;
        }
    }
    return flag;
}
