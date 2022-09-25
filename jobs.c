#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

struct background_process
{
    int pid;
    char name[500];
    int s_no;
    // int status;
};
// struct background_process back_proc[500];
struct parsed_comm
{
    char command[500];
    int isBackgroundProcess;
};

// extern struct background_process back_proc[500];

void jobs(char **args, struct background_process back_proc[500])
{
    int alphabetic_indices[500];
    int i, j = 0;
    for (i = 0; i < 500; i++)
    {
        if (back_proc[i].pid != -1)
        {
            // printf("[%d] %s [%d] \n", back_proc[i].s_no, back_proc[i].name, back_proc[i].pid);
            alphabetic_indices[j] = i;
            j++;
        }
    }
    // sort the indices in alphabetic order
    for (i = 0; i < j; i++)
    {
        for (int k = i + 1; k < j; k++)
        {
            if (strcmp(back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[k]].name) > 0)
            {
                int temp = alphabetic_indices[i];
                alphabetic_indices[i] = alphabetic_indices[k];
                alphabetic_indices[k] = temp;
            }
        }
    }
    int rflag = 0;
    int sflag = 0;
    if (args[1] != NULL && args[1][0] == '-')
    {
        if (args[1][1] == 'r')
        {
            rflag = 1;
        }
        else if (args[1][1] == 's')
        {
            sflag = 1;
        }
        else
        {
            printf("Error: Invalid flag\n");
            return;
        }
    }
    // read the status of each process from /proc/pid/stat and print along with other details in back_proc
    for (i = 0; i < j; i++)
    {
        int pid = back_proc[alphabetic_indices[i]].pid;
        char proc_stat_path[500];
        sprintf(proc_stat_path, "/proc/%d/stat", pid);

        FILE *fd1 = fopen(proc_stat_path, "r");

        if (fd1 == NULL)
        {
            printf("Error: cannot get information about process with pid %d\n", pid);
            // return;
            continue;
        }

        // get state of the process from /proc/<pid>/stat
        char state = 0;
        fscanf(fd1, "%*d %*s %c", &state);
        fclose(fd1);
        char status[10];
        if (state == 'S' || state == 'R')
        {
            strcpy(status, "Running");
        }
        else if (state == 'T')
        {
            strcpy(status, "Stopped");
        }
        else if (state == 'Z')
        {
            strcpy(status, "Zombie");
        }
        else
        {
            strcpy(status, "Unknown");
        }

        if (rflag == 1)
        {
            if (strcmp(status, "Running") == 0)
            {
                // printf("[%d] %s [%d] %s\n", back_proc[alphabetic_indices[i]].s_no, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid, status);
                printf("[%d] %s %s [%d]  \n", back_proc[alphabetic_indices[i]].s_no, status, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid);
            }
        }
        else if (sflag == 1)
        {
            if (strcmp(status, "Stopped") == 0)
            {
                // printf("[%d] %s [%d] %s\n", back_proc[alphabetic_indices[i]].s_no, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid, status);
                printf("[%d] %s %s [%d]  \n", back_proc[alphabetic_indices[i]].s_no, status, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid);
            }
        }
        else
        {
            // printf("[%d] %s [%d] %s\n", back_proc[alphabetic_indices[i]].s_no, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid, status);
            printf("[%d] %s %s [%d]  \n", back_proc[alphabetic_indices[i]].s_no, status, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid);
        }

        // printf("[%d] %s %s [%d]  \n", back_proc[alphabetic_indices[i]].s_no, status, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid);
    }

    // for (i = 0; i < j; i++)
    // {
    //     printf("[%d] %s [%d] \n", back_proc[alphabetic_indices[i]].s_no, back_proc[alphabetic_indices[i]].name, back_proc[alphabetic_indices[i]].pid);
    // }
}


void sig(char** args, struct background_process back_proc[500])
{
    int proc_s_no = atoi(args[1]);
    int sign = atoi(args[2]);

    // find proc whose index is s_no and send signal sig to it
    int i;
    for (i = 0; i < 500; i++)
    {
        if (back_proc[i].s_no == proc_s_no)
        {
            break;
        }
    }
    if (i == 500)
    {
        printf("Error: No such process\n");
        return;
    }
    int pid = back_proc[i].pid;
    if (kill(pid, sign) == -1)
    {
        printf("Error: Cannot send signal %d to process with pid %d\n", sign, pid);
    }
}

void bg(char ** args, struct background_process back_proc[500])
{
    int proc_s_no = atoi(args[1]);

    // find proc whose index is s_no and send signal sig to it
    int i;
    for (i = 0; i < 500; i++)
    {
        if (back_proc[i].s_no == proc_s_no)
        {
            break;
        }
    }
    if (i == 500)
    {
        printf("Error: No such process\n");
        return;
    }
    int pid = back_proc[i].pid;
    int v = kill(pid, SIGTTIN);
    if (v == -1)
    {
        printf("Error: Cannot send signal SIGTTIN to process with pid %d\n", pid);
        return;
    }
    int check_val = kill(pid, SIGCONT);
    if (check_val == -1)
    {
        printf("Error: Cannot send signal %d to process with pid %d\n", SIGCONT, pid);
    }
}

void fg(char ** args, struct background_process back_proc[500])
{
    int proc_s_no = atoi(args[1]);

    // find proc whose index is s_no and send signal sig to it
    int i;
    for (i = 0; i < 500; i++)
    {
        if (back_proc[i].s_no == proc_s_no)
        {
            break;
        }
    }
    if (i == 500)
    {
        printf("Error: No such process\n");
        return;
    }
    int pid = back_proc[i].pid;
    int v = kill(pid, SIGTTIN);
    if (v == -1)
    {
        printf("Error: Cannot send signal SIGTTIN to process with pid %d\n", pid);
        return;
    }
    int check_val = kill(pid, SIGCONT);
    if (check_val == -1)
    {
        printf("Error: Cannot send signal %d to process with pid %d\n", SIGCONT, pid);
    }
    // remove from background process list
    back_proc[i].s_no = -1;
    back_proc[i].pid = -1;
    int status;
    waitpid(pid, &status, WUNTRACED);
    if (WIFSTOPPED(status))
    {
        printf("Process with pid %d stopped\n", pid);
        back_proc[i].s_no = proc_s_no;
        back_proc[i].pid = pid;
        strcpy(back_proc[i].name, args[0]);
        
    }
}