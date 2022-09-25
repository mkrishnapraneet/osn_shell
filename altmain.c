#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <termios.h>
// #include <parse.c>

#define RED "\x1B[1;31m"
#define GRN "\x1B[1;32m"
#define YEL "\x1B[1;33m"
#define BLU "\x1B[1;34m"
#define MAG "\x1B[1;35m"
#define CYN "\x1B[1;36m"
#define WHT "\x1B[1;37m"
#define RESET "\x1B[1;0m"

int active_pid = -1;
int bg_counter = 0;
int parent_pid;
FILE *hist_file;
// char history[20][500];
struct background_process
{
    int pid;
    char name[500];
    int s_no;
    // int status;
};
struct background_process back_proc[500];
struct parsed_comm
{
    char command[500];
    int isBackgroundProcess;
};

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int parse(char *str, char commands[500][500], struct parsed_comm parsed_commands[500], char background[500][500], char init_dir[500], char old_wd[500], char history[20][500], struct background_process back_proc[500]);

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
        // char buffer[500];
        // sprintf(buffer, "/proc/%d/cmdline", pid);
        // FILE *fd = fopen(buffer, "r");
        // if (fd == NULL)
        // {
        //     printf("Error: %s\n", strerror(errno));
        //     return;
        // }
        // char *pname;
        // long size = 0;
        // getline(&pname, &size, fd);
        // char pname[500];
        // fgets(pname, 500, fd);
        if (WIFEXITED(status))
        {
            // iterate over the background processes and find the one with the pid
            int i = 0;
            while (back_proc[i].pid != pid)
            {
                i++;
            }

            // printf("\n%s with PID %d exited normally with status %d\n", pname, pid, WEXITSTATUS(status));
            printf("\n%s with PID %d exited normally with status %d\n", back_proc[i].name, pid, WEXITSTATUS(status));
            back_proc[i].pid = -1;
        }
        else if (WIFSIGNALED(status))
        {
            // iterate over the background processes and find the one with the pid
            int i = 0;
            while (back_proc[i].pid != pid)
            {
                i++;
            }

            // printf("\n%s with PID %d exited due to signal %d\n", pname, pid, WTERMSIG(status));
            printf("\n%s with PID %d exited due to signal %d\n", back_proc[i].name, pid, WTERMSIG(status));
            back_proc[i].pid = -1;
        }
    }
}

void CTRL_C_handler(int sign)
{
    // printf("detected ctrc c\n");
    // if (getpid() == parent_pid)
    // {
    //     // printf("tis\n");
    //     if (active_pid != -1)
    //     {
    //         // check if it is a background process
    //         // int i = 0;
    //         // while (back_proc[i].pid != active_pid && i < 500)
    //         // {
    //         //     i++;
    //         // }
    //         // if (i >= 500) // if it is not a background process
    //         // {
    //         printf("killing %d", active_pid);
    //         // kill(active_pid, SIGINT);
    //         // }
    //     }
    //     // signal(SIGINT, CTRL_C);
    //     // printf
    // }

    if (getpid() != parent_pid)
        return;

    if (active_pid != -1)
    {
        kill(active_pid, SIGINT);
    }
    return;
    // signal(SIGINT, CTRL_C_handler);
}

void CTRL_Z_handler(int signal)
{
    printf("\ndetected ctrl z\n");
}

void CTRL_C_tester(int sign)
{
    signal(SIGINT, SIG_IGN);
    printf("detected ctrl c\n");
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

    // struct background_process background_processes[500];
    // allocate memory for background processes dynamically

    // initialize background processes
    for (int i = 0; i < 500; i++)
    {
        back_proc[i].pid = -1;
        strcpy(back_proc[i].name, "");
        back_proc[i].s_no = -1;
    }

    getcwd(init_dir, sizeof(init_dir));
    if (init_dir == NULL)
    {
        perror("getcwd() error : ");
        return;
    }
    strcpy(old_wd, init_dir);

    signal(SIGTSTP, CTRL_Z_handler);
    // signal(SIGINT, CTRL_C_handler);
    // signal(SIGINT, SIG_IGN);
    // signal(SIGINT, CTRL_C_tester);
    // signal(SIGINT, SIG_IGN);

    signal(SIGCHLD, sigchld_handler);

    // hist_file = fopen("history.txt", "w");

    // hist_file = fopen("history.txt", "a+");
    // if (hist_file == NULL)
    // {
    //     perror("fopen() error");
    //     return;
    // }

    // get username from system using getpwuid()
    char *inp = malloc(sizeof(char) * 100);
    char c;
    while (1)
    {
        setbuf(stdout, NULL);
        enableRawMode();
        // printf("Prompt>");
        memset(inp, '\0', 100);
        int pt = 0;
        // signal(SIGINT, SIG_IGN);
        // signal(SIGINT, CTRL_C_tester);
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

        active_pid = -1;
        // get current time

        // if (strcmp(cwd, init_dir) == 0)
        // strcpy(cwd, "~");
        printf(CYN "<< " RESET RED "%s" RESET "@" BLU "%s" RESET ":" YEL "%s" RESET " || " MAG "%s" RESET CYN " >> " RESET, username, hostname, print_cwd, time_diff_str);

        // read input from user using getline()
        char *input = malloc(500 * sizeof(char));
        int bufsize = 500;
        size_t size = 0;
        int ctrld = 0;

        // char c;
        // int i = 0;
        // while ((c = getchar()) != '\n')
        // {
        //     if (c == EOF)
        //     {
        //         ctrld = -1;
        //         break;
        //     }
        //     if (c == '\t')
        //     {
        //         printf("tab detected");
        //         // ctrld = -1;
        //         // continue;
        //         break;
        //     }
        //     input[i] = c;
        //     i++;

        //     if (i >= bufsize)
        //     {
        //         bufsize += 500;
        //         input = realloc(input, bufsize * sizeof(char));
        //         if (input == NULL)
        //         {
        //             perror("realloc() error");
        //             exit(1);
        //         }
        //     }
        // }
        // input[i] = '\0';
        // if (input == NULL)
        // {
        //     perror("getline() error");
        //     return;
        // }
        // if (ctrld == -1)
        // {
        //     input = "exit";
        // }

        

        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (iscntrl(c))
            {
                if (c == 10)
                    break;
                else if (c == 27)
                {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2)
                    { // length of escape code
                        // printf("\rarrow key: %s", buf);
                    }
                }
                else if (c == 127)
                { // backspace
                    if (pt > 0)
                    {
                        if (inp[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        inp[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (c == 9)
                { // TAB character
                    inp[pt++] = c;
                    for (int i = 0; i < 8; i++)
                    { // TABS should be 8 spaces
                        printf(" ");
                    }
                }
                else if (c == 4)
                {
                    exit(0);
                }
                else
                {
                    printf("%d\n", c);
                }
            }
            else
            {
                inp[pt++] = c;
                printf("%c", c);
            }
        }
        disableRawMode();

        signal(SIGINT, CTRL_C_tester);

        t1 = time(NULL);
        flag = parse(input, commands, parsed_commands, background, init_dir, old_wd, history, back_proc);

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