#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// #include <parse.c>


#define RED   "\x1B[1;31m"
#define GRN   "\x1B[1;32m"
#define YEL   "\x1B[1;33m"
#define BLU   "\x1B[1;34m"
#define MAG   "\x1B[1;35m"
#define CYN   "\x1B[1;36m"
#define WHT   "\x1B[1;37m"
#define RESET "\x1B[1;0m"

void parse(char* str, char commands[500][500], char background[500][500]);


void mainloop()
{
    char username[500];
    char hostname[500];
    char init_dir[500];
    char cwd[500];
    char print_cwd[500];
    char commands[500][500];
    char background[500][500];
    char* replace;

    getcwd(init_dir, sizeof(init_dir));

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

        // get current working directory using getcwd()
        getcwd(cwd, sizeof(cwd));
        strcpy(print_cwd, cwd);
        replace = strstr(print_cwd, init_dir);
        if (replace != NULL)
        {   
            strcpy(replace, "~\0");
        }
        // if (strcmp(cwd, init_dir) == 0)
            // strcpy(cwd, "~");
        printf(CYN "<< " RESET RED "%s" RESET "@" BLU "%s" RESET ":" YEL "%s" RESET CYN " >> " RESET, username, hostname, print_cwd);

        // read input from user using getline()
        char* input;
        size_t size = 0;
        getline(&input, &size, stdin);
        parse(input, commands, background);
        
        printf("\n");

    }

    return;
}

int main()
{
    mainloop();
    return 0;
}