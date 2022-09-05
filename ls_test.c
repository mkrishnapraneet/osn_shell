#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"

void ls_test(char args[100][50], char init_dir[500])
{
    struct dirent *de; // Pointer for directory entry
    struct dirent *de2;

    int going = 1;
    int a_flag = 0;
    int l_flag = 0;
    char paths[100][500] = {"\0"};
    int num_paths = 0;
    strcpy(paths[0], ".");
    while (args[going][0] != '\0')
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
        else if ((args[going][0] != '~') && (args[going][0] != '/'))
        {
            strcpy(paths[num_paths], "./");
            strcat(paths[num_paths], args[going]);
            num_paths++;
            // printf("Error : Invalid flag\n");
            // return;
        }
        else if (args[going][0] == '~')
        {
            strcpy(paths[num_paths], init_dir);
            strcat(paths[num_paths], args[going] + 1);
            num_paths++;
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
        // print the names
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", files[i]);
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
                // printf(BOLD "%s" RESET ": \n", paths[b]);
                printf("%s\n\n", paths[b]);
            }
                
        }
        b++;
    }

    return;
}