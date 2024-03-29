#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"
#define RED "\x1B[1;31m"
#define GRN "\x1B[1;32m"
#define YEL "\x1B[1;33m"
#define BLU "\x1B[1;34m"
#define MAG "\x1B[1;35m"
#define CYN "\x1B[1;36m"
#define WHT "\x1B[1;37m"

void ls(char **args, char init_dir[500])
{
    struct dirent *de; // Pointer for directory entry
    struct dirent *de2;

    int going = 1;
    int a_flag = 0;
    int l_flag = 0;
    char paths[100][500] = {"\0"};
    int num_paths = 0;
    strcpy(paths[0], ".");
    while (args[going] != NULL)
    {
        if (args[going][0] == '-')
        {
            int r = 1;
            while (args[going][r] != '\0')
            {
                if (args[going][r] == 'a')
                {
                    a_flag = 1;
                }
                else if (args[going][r] == 'l')
                {
                    l_flag = 1;
                }
                else
                {
                    printf("Error : Invalid flag\n");
                    return;
                }
                r++;
            }
        }
        else if (args[going][0] == '~')
        {
            strcpy(paths[num_paths], init_dir);
            strcat(paths[num_paths], args[going] + 1);
            num_paths++;
        }
        else if (args[going][0] == '.' && args[going][1] == '/')
        {
            // strcpy(paths[num_paths], "./");
            strcat(paths[num_paths], args[going]);
            num_paths++;
            // printf("Error : Invalid flag\n");
            // return;
        }
        
        else
        {
            strcpy(paths[num_paths], args[going]);
            num_paths++;
        }
        going++;
    }
    int b = 0;
    while (paths[b][0] != '\0')
    {

        // DIR *dr = opendir(".");
        DIR *dr = opendir(paths[b]);
    
        int is_file = 0;

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            // printf("Could not open directory");
            // return;
            FILE *f_ret = fopen(paths[b], "r");
            if (f_ret == NULL)
            {
                printf("Error : No such file or directory\n");
                return;
            }
            else
            {
                is_file = 1;
                fclose(f_ret);
                // printf("yes\n");
            }
        }

        // DIR *dummy = opendir(".");
        if (is_file == 0)
        {
            DIR *dummy = opendir(paths[b]);

            int count = 0;
            if (a_flag == 1)
            {
                while ((de2 = readdir(dummy)) != NULL)
                {
                    count++;
                }
            }
            else
            {
                while ((de2 = readdir(dummy)) != NULL)
                {
                    if (de2->d_name[0] != '.')
                    {
                        count++;
                    }
                }
            }

            closedir(dummy);

            char files[count][500];

            // char obj[500];
            int iter = 0;

            if (a_flag == 1)
            {
                while ((de = readdir(dr)) != NULL)
                {

                    // printf("%s\n", de->d_name);
                    strcpy(files[iter], de->d_name);
                    iter++;
                }
            }

            else
            {
                while ((de = readdir(dr)) != NULL) // returns pointer to next element in the directory stream
                {
                    // obj = de->d_name;
                    if (de->d_name[0] != '.')
                    {
                        // printf("%s\n", de->d_name);
                        strcpy(files[iter], de->d_name);
                        iter++;
                    }
                }
            }

            closedir(dr);
            char temp1[500], temp2[500];
            // sort the names in alphabetical order
            for (int i = 0; i < count; i++)
            {
                for (int j = i + 1; j < count; j++)
                {
                    int k = 0;
                    while (files[i][k] != '\0')
                    {
                        temp1[k] = tolower(files[i][k]);
                        k++;
                    }

                    int l = 0;
                    while (files[j][l] != '\0')
                    {
                        temp2[l] = tolower(files[j][l]);
                        l++;
                    }

                    if (strcmp(temp1, temp2) > 0)
                    {
                        char temp[500];
                        strcpy(temp, files[i]);
                        strcpy(files[i], files[j]);
                        strcpy(files[j], temp);
                    }
                }
            }

            printf(BOLD "%s" RESET ": \n", paths[b]);
            char temp_path[500];

            // print the names
            if (l_flag == 1)
            {
                char date[100];
                for (int i = 0; i < count; i++)
                {
                    strcpy(temp_path, paths[b]);
                    strcat(temp_path, "/");
                    strcat(temp_path, files[i]);
                    struct stat fileStat;
                    if (stat(temp_path, &fileStat) < 0)
                    {
                        printf("Error : No such file or directory\n");
                        return;
                    }
                    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\t%ld\t", fileStat.st_nlink);
                    struct passwd *pw = getpwuid(fileStat.st_uid);
                    struct group *gr = getgrgid(fileStat.st_gid);
                    printf("%s %s\t", pw->pw_name, gr->gr_name);
                    printf("\t%ld\t", fileStat.st_size);
                    // char *time = ctime(&fileStat.st_mtime);
                    // time[strlen(time) - 1] = '\0';
                    // printf("%s ", time);
                    struct tm *time = localtime(&fileStat.st_mtime);
                    // printf("%2d %2d %2d:%2d ", time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min);

                    strftime(date, 10, "%b", time);
                    printf("\t%s  %2d", date, time->tm_mday);
                    printf("  %02d:%02d\t", time->tm_hour, time->tm_min);
                    if ((S_ISDIR(fileStat.st_mode)))
                    {
                        printf(BOLD BLU "\t%s" RESET "\n", files[i]);
                    }
                    else
                    {
                        if ((fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_IXOTH))
                        {
                            printf(BOLD GRN "\t%s" RESET "\n", files[i]);
                        }
                        else
                        {
                            printf("\t%s\n", files[i]);
                        }
                    }
                }
            }
            else
            {
                char temp_path[500];
                for (int i = 0; i < count; i++)
                {
                    struct stat fileStat;
                    strcpy(temp_path, paths[b]);
                    strcat(temp_path, "/");
                    strcat(temp_path, files[i]);
                    stat(temp_path, &fileStat);
                    if ((S_ISDIR(fileStat.st_mode)))
                    {
                        printf(BOLD BLU "%s" RESET "\n", files[i]);
                    }
                    else
                    {
                        if ((fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_IXOTH))
                        {
                            printf(BOLD GRN "%s" RESET "\n", files[i]);
                        }
                        else
                        {
                            printf("%s\n", files[i]);
                        }
                        // printf("%s\n", files[i]);
                    }
                }
            }

            printf("\n");
        }
        else
        {
            struct stat fileStat;
            if (stat(paths[b], &fileStat) < 0)
            {
                printf("Error : No such file or directory\n");
                return;
            }
            else
            {
                if (l_flag == 1)
                {
                    char date[100];
                    // char temp_path[500];
                    // strcpy(temp_path, paths[b]);
                    // strcat(temp_path, "/");
                    // // strcat(temp_path, files[i]);
                    // // struct stat fileStat;
                    // if (stat(temp_path, &fileStat) < 0)
                    // {
                    //     printf("Error : No such file or directory\n");
                    //     return;
                    // }
                    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\t%ld\t", fileStat.st_nlink);
                    struct passwd *pw = getpwuid(fileStat.st_uid);
                    struct group *gr = getgrgid(fileStat.st_gid);
                    printf("%s %s\t", pw->pw_name, gr->gr_name);
                    printf("\t%ld\t", fileStat.st_size);
                    // char *time = ctime(&fileStat.st_mtime);
                    // time[strlen(time) - 1] = '\0';
                    // printf("%s ", time);
                    struct tm *time = localtime(&fileStat.st_mtime);
                    // printf("%2d %2d %2d:%2d ", time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min);

                    strftime(date, 10, "%b", time);
                    printf("\t%s  %2d", date, time->tm_mday);
                    printf("  %02d:%02d\t", time->tm_hour, time->tm_min);
                    if ((fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_IXOTH))
                    {
                        printf(BOLD GRN "\t%s" RESET "\n\n", paths[b]);
                    }
                    else
                    {
                        printf("\t%s\n\n", paths[b]);
                    }
                    // printf("\t%s\n\n", paths[b]);
                }
                else
                {
                    // printf(BOLD "%s" RESET ": \n", paths[b]);
                    // printf("%s\n\n", paths[b]);
                    if ((fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_IXOTH))
                    {
                        printf(BOLD GRN "%s" RESET "\n\n", paths[b]);
                    }
                    else
                    {
                        printf("%s\n\n", paths[b]);
                    }
                }
            }
        }
        b++;
    }

    return;
}