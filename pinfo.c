#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

void pinfo(char **args, char init_dir[500])
{
    int pid = getpid();
    char proc_status[10] = "R+";
    int mem = 0;
    char path[500];

    int ch = 1;
    while (args[ch] != NULL)
    {
        ch++;
    }

    if (ch > 2)
    {
        printf("Error: Too many arguments\n");
    }

    if (args[1] != NULL)
    {
        for (int i = 0; i < strlen(args[1]); i++)
        {
            if (args[1][i] < '0' || args[1][i] > '9')
            {
                printf("Error: Invalid PID\n");
                return;
            }
        }

        pid = atoi(args[1]);
    }

    printf("pid : \t%d\n", pid);

    // process status
    // char buf[500];
    char proc_stat_path[500];
    sprintf(proc_stat_path, "/proc/%d/stat", pid);

    FILE *fd1 = fopen(proc_stat_path, "r");

    if (fd1 == NULL)
    {
        printf("Error: cannot get information about process with pid %d\n", pid);
        return;
    }

    // get state of the process from /proc/<pid>/stat
    char state;
    fscanf(fd1, "%*d %*s %c", &state);

    printf("Process State : %c\n", state);
    fclose(fd1);

    char buffer[500];
    char *temp;
    long size = 0;
    sprintf(buffer, "/proc/%d/status", pid);
    FILE *fd = fopen(buffer, "r");
    if (fd == NULL)
    {
        printf("Error: while retrieving information about required process with pid %d\n", pid);
        return;
    }
    // printf("pid -- %d\n", pid); // PID value

    for (int i = 0; i < 3; ++i) // Status present on 3rd line
        getline(&temp, &size, fd);
    // printf("%s", temp);

    for (int i = 0; i < 15; i++) // Memory allocation
        getline(&temp, &size, fd);
    printf("%s", temp);

    fclose(fd);

    // executable path
    char proc_exe_path[500];

    sprintf(proc_exe_path, "/proc/%d/exe", pid);

    char exec_path[500];

    int val = readlink(proc_exe_path, exec_path, 500);

    if (val == -1)
    {
        printf("Error getting executable path for process with pid: %d\n", pid);
    }

    char *ptr = NULL;
    ptr = strstr(exec_path, init_dir);
    if (ptr != NULL)
    {
        printf("Executable Path: ~%s\n", ptr + strlen(init_dir));
    }
    else
    {
        printf("Executable Path: %s\n", exec_path);
    }
    // ptr = ptr + strlen(init_dir);

    // printf("Executable Path: %s\n", exec_path);
}