#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
// #include <parse.c>

#define RED "\x1B[1;31m"
#define GRN "\x1B[1;32m"
#define YEL "\x1B[1;33m"
#define BLU "\x1B[1;34m"
#define MAG "\x1B[1;35m"
#define CYN "\x1B[1;36m"
#define WHT "\x1B[1;37m"
#define RESET "\x1B[1;0m"

int parent_pid;
FILE *hist_file;
// char history[20][500];

struct parsed_comm
{
    char command[500];
    int isBackgroundProcess;
};

int parse(char *str, char commands[500][500], struct parsed_comm parsed_commands[500], char background[500][500], char init_dir[500], char old_wd[500], char history[20][500]);

// void sig_end_handler(int sig_num, char history[20][500])
// {
//     // printf("\n");
//     // fflush(stdout);
//     printf("YOOO\n");
//     hist_file = fopen("history.txt", "w");

//     for (int k = 0;k < 20; k++)
//     {
//         fprintf(hist_file, "%s", history[k]);

//     }
//     fclose(hist_file);
// }

void sigchld_handler(int sign)
{
    if (getpid() != parent_pid)
    {
        return;
    }

    int status;
    int pid = waitpid(-1, &status, WNOHANG); // WNOHANG is used so that wait doesn't block the program
    if (pid > 0)
    {
        if (WIFEXITED(status))
        {
            printf("\nProcess with PID %d exited normally with status %d\n", pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("\nProcess with PID %d exited due to signal %d\n", pid, WTERMSIG(status));
        }
    }
}

void mainloop(char history[20][500])
{
    char username[500];
    char hostname[500];
    char init_dir[500];
    char cwd[500];
    char print_cwd[500];
    char old_wd[500];
    char commands[500][500];
    char background[500][500];
    char *replace;
    int flag = 0;
    struct parsed_comm parsed_commands[500];
    time_t t1 = time(NULL);
    time_t t2 = time(NULL);
    int time_diff = 0;
    char time_diff_str[500] = "";

    char prev_command[500] = "";

    getcwd(init_dir, sizeof(init_dir));
    if (init_dir == NULL)
    {
        perror("getcwd() error : ");
        return;
    }
    strcpy(old_wd, init_dir);

    signal(SIGCHLD, sigchld_handler);

    // hist_file = fopen("history.txt", "w");

    // hist_file = fopen("history.txt", "a+");
    // if (hist_file == NULL)
    // {
    //     perror("fopen() error");
    //     return;
    // }

    // get username from system using getpwuid()
    while (1)
    {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            strcpy(username, pw->pw_name);
        else
            strcpy(username, "unknown");

        // get hostname from system using gethostname()
        gethostname(hostname, sizeof(hostname));
        if (hostname == NULL)
        {
            perror("gethostname");
            exit(1);
        }

        // get current working directory using getcwd()
        getcwd(cwd, sizeof(cwd));
        if (cwd == NULL)
        {
            perror("getcwd");
            exit(1);
        }
        strcpy(print_cwd, cwd);
        replace = strstr(print_cwd, init_dir);
        if (replace != NULL)
        {
            char *temp = replace;
            temp = temp + strlen(init_dir);
            strcpy(print_cwd, "~");
            strcat(print_cwd, temp);
        }
        // get current time

        // if (strcmp(cwd, init_dir) == 0)
        // strcpy(cwd, "~");
        printf(CYN "<< " RESET RED "%s" RESET "@" BLU "%s" RESET ":" YEL "%s" RESET " || " MAG "%s" RESET CYN " >> " RESET, username, hostname, print_cwd, time_diff_str);

        // read input from user using getline()
        char *input;
        size_t size = 0;
        getline(&input, &size, stdin);
        if (input == NULL)
        {
            perror("getline() error");
            return;
        }

        // strcpy(input, input);
        // if (strcmp(input, prev_command) != 0)
        // {
        //     // printf("--%s %s--", input, prev_command);
        //     // store input in history file
        //     fprintf(hist_file, "%s", input);
        //     fflush(hist_file);
        // }

        t1 = time(NULL);
        flag = parse(input, commands, parsed_commands, background, init_dir, old_wd, history);

        // strcpy(prev_command, input);
        // printf("$$%s %s$$", input, prev_command);

        if (flag == 1)
        {
            strcpy(old_wd, cwd);
        }

        if (strcmp(input, history[0]) != 0)
        {
            for (int o = 19; o > 0; o--)
            {
                strcpy(history[o], history[o - 1]);
            }
            strcpy(history[0], input);

            // for (int x = 0; x < 20; x++)
            // {
            //     printf("%s %ld\n", history[x], strlen(history[x]));
            // }

            hist_file = fopen("history.txt", "w");
            for (int g = 0; g < 20; g++)
            {
                fprintf(hist_file, "%s\n", history[g]);
            }
            fclose(hist_file);
        }

        printf("\n");
        t2 = time(NULL);
        time_diff = t2 - t1;

        if (time_diff > 1)
        {
            sprintf(time_diff_str, "%d", time_diff);
            strcat(time_diff_str, " seconds");
        }
        else
        {
            strcpy(time_diff_str, "");
        }
    }

    return;
}

int main()
{
    parent_pid = getpid();
    char history[20][500];

    for (int i = 0; i < 20; i++)
    {
        strcpy(history[i], "");
    }

    hist_file = fopen("history.txt", "r+");
    if (hist_file == NULL)
    {
        // perror("fopen() error");
        // return -1;
        hist_file = fopen("history.txt", "w");
        fclose(hist_file);
        hist_file = fopen("history.txt", "r+");
    }

    int i = 0;
    while (i < 20)
    {
        (fgets(history[i], 500, hist_file));
        history[i][strlen(history[i]) - 1] = '\0';
        // strcpy(history[i], );
        i++;
    }
    // for (int k = 0; k < 20; k++)
    // {
    //     printf("%s\n", history[k]);
    // }
    fclose(hist_file);

    // signal(SIGINT, sig_end_handler(history));

    mainloop(history);

    return 0;
}