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

int list_matching_components(char target_dir[500], char file_to_search[500], char all_comps[500][500], char comps_to_rep[500][500], int index)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char path[500];
    char *file_name;

    if (!(dir = opendir(target_dir)))
        return index;

    while ((entry = readdir(dir)) != NULL)
    {
        file_name = entry->d_name;

        // skip the current and parent directory
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
            continue;

        // get the full path of the file
        sprintf(path, "%s/%s", target_dir, file_name);

        // get the file stat
        if (lstat(path, &file_stat) < 0)
        {
            printf("Error:\n");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            if (strcmp(file_name, file_to_search) == 0)
            {
                strcpy(comps_to_rep[index], path);
                index++;
            }
            else
            {
                index = list_matching_components(path, file_to_search, all_comps, comps_to_rep, index);
            }
            // strcpy(all_comps[index], path);
            // index++;

            index = list_matching_components(path, file_to_search, all_comps, comps_to_rep, index);
        }
        else
        {
            strcpy(all_comps[index], path);
            index++;
            // return;
        }
    }
    closedir(dir);
    return index;
}

int list_all_components(char target_dir[500], char all_comps[500][500], int index)
{
    // list all components in the target directory recursively
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char path[500];
    char *file_name;

    if (!(dir = opendir(target_dir)))
        return index;

    while ((entry = readdir(dir)) != NULL)
    {
        file_name = entry->d_name;

        // skip the current and parent directory
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0)
            continue;

        // get the full path of the file
        sprintf(path, "%s/%s", target_dir, file_name);

        // get the file stat
        if (lstat(path, &file_stat) < 0)
        {
            printf("Error:\n");
            continue;
        }

        if (S_ISDIR(file_stat.st_mode))
        {
            strcpy(all_comps[index], path);
            index++;
            index = list_all_components(path, all_comps, index);
        }
        else
        {
            strcpy(all_comps[index], path);
            index++;
            // return;
        }
    }
    closedir(dir);
    return index;
}

void discover(char **args, char init_dir[500])
{
    struct dirent *de; // Pointer for directory entry
    struct dirent *de2;

    int going = 1;
    int d_flag = 0;
    int f_flag = 0;
    // char paths[100][500] = {"\0"};
    // int num_paths = 0;
    char target_dir[500] = ".";
    char file_to_search[500] = "\0";
    // strcpy(paths[0], ".");
    while (args[going] != NULL)
    {
        if (args[going][0] == '-')
        {
            int r = 1;
            while (args[going][r] != '\0')
            {
                if (args[going][r] == 'd')
                {
                    d_flag = 1;
                }
                else if (args[going][r] == 'f')
                {
                    f_flag = 1;
                }
                else
                {
                    printf("Error : Invalid flag\n");
                    return;
                }
                r++;
            }
        }
        else if (args[going][0] == '"')
        {
            // strcpy(paths[num_paths], args[going]);
            // num_paths++;
            // printf("YES\n");
            strcpy(file_to_search, args[going] + 1);
            file_to_search[strlen(file_to_search) - 1] = '\0';
        }
        else if (args[going][0] == '~')
        {
            strcpy(target_dir, init_dir);
            strcat(target_dir, args[going] + 1);
            // num_paths++;
        }
        else if ((args[going][0] == '.' && (args[going][1] == '/')))
        {
            // strcpy(target_dir, "./");
            strcpy(target_dir, args[going]);
            // num_paths++;
            // printf("Error : Invalid flag\n");
            // return;
        }
        else
        {
            strcpy(target_dir, args[going]);
        }
        going++;
    }

    char all_comps[500][500] = {"\0"};
    char comps_to_rep[500][500] = {"\0"};

    if (d_flag == 0 && f_flag == 0)
    {
        if (strcmp(file_to_search, "\0") == 0)
        {
            // list all files recursively
            list_all_components(target_dir, all_comps, 0);
            // print all_comps
            int i = 0;
            while (strcmp(all_comps[i], "\0") != 0)
            {
                printf("%s\n", all_comps[i]);
                i++;
            }
        }

        else
        {
            // list all files recursively with the same name as file_to_search
            // list_matching_components(target_dir, file_to_search, all_comps, comps_to_rep, 0);
            list_all_components(target_dir, all_comps, 0);
            // print ones that end with file_to_search
            int i = 0;
            while (strcmp(all_comps[i], "\0") != 0)
            {
                int len = strlen(all_comps[i]);
                int len2 = strlen(file_to_search);
                if (len >= len2)
                {
                    if ((strcmp(all_comps[i] + len - len2, file_to_search) == 0) && (all_comps[i][len - len2 - 1] == '/'))

                    {
                        printf("%s\n", all_comps[i]);
                    }
                }
                i++;
            }
        }
    }

    return;
}