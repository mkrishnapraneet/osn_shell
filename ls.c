#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

void ls(char args[100][50])
{
    struct dirent *de; // Pointer for directory entry
    struct dirent *de2;

    if (args[1][0] == '\0')
    {
        // opendir() returns a pointer of DIR type.
        DIR *dr = opendir(".");

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            printf("Could not open directory");
            return;
        }

        DIR *dummy = opendir(".");

        int count = 0;
        while ((de2 = readdir(dummy)) != NULL)
        {
            if (de2->d_name[0] != '.')
            {
                count++;
            }
        }
        closedir(dummy);

        char files[count][500];

        // char obj[500];
        int iter = 0;
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

        // print the names
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", files[i]);
        }
    }

    else if (strcmp(args[1], ".") == 0 && args[2][0] == '\0')
    {

        // opendir() returns a pointer of DIR type.
        DIR *dr = opendir(".");

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            printf("Could not open directory");
            return;
        }

        DIR *dummy = opendir(".");

        int count = 0;
        while ((de2 = readdir(dummy)) != NULL)
        {
            if (de2->d_name[0] != '.')
            {
                count++;
            }
        }
        closedir(dummy);

        char files[count][500];

        // char obj[500];
        int iter = 0;
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

        // print the names
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", files[i]);
        }
    }

    else if (strcmp(args[1], "..") == 0 && args[2][0] == '\0')
    {
        DIR *dr = opendir("..");

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            printf("Could not open directory");
            return;
        }

        DIR *dummy = opendir("..");

        int count = 0;
        while ((de2 = readdir(dummy)) != NULL)
        {
            if (de2->d_name[0] != '.')
            {
                count++;
            }
        }
        closedir(dummy);

        char files[count][500];

        // char obj[500];
        int iter = 0;
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

        // print the names
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", files[i]);
        }

        // while ((de = readdir(dr)) != NULL) // returns pointer to next element in the directory stream
        // {
        //     // obj = de->d_name;
        //     if (de->d_name[0] != '.')
        //     {
        //         printf("%s\n", de->d_name);
        //     }
        // }

        // closedir(dr);
    }

    else if (strcmp(args[1], "-a") == 0 && args[2][0] == '\0')
    {
        // DIR *dr = opendir(".");

        // if (dr == NULL) // opendir returns NULL if couldn't open directory
        // {
        //     printf("Could not open directory");
        //     return;
        // }

        // while ((de = readdir(dr)) != NULL) // returns pointer to next element in the directory stream
        // {
        //     printf("%s\n", de->d_name);
        // }

        // opendir() returns a pointer of DIR type.
        DIR *dr = opendir(".");

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            printf("Could not open directory");
            return;
        }

        DIR *dummy = opendir(".");

        int count = 0;
        while ((de2 = readdir(dummy)) != NULL)
        {
            // if (de2->d_name[0] != '.')
            // {
            count++;
            // }
        }
        closedir(dummy);

        char files[count][500];

        // char obj[500];
        int iter = 0;
        while ((de = readdir(dr)) != NULL) // returns pointer to next element in the directory stream
        {
            // obj = de->d_name;
            // if (de->d_name[0] != '.')
            // {
            // printf("%s\n", de->d_name);
            strcpy(files[iter], de->d_name);
            iter++;
            // }
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

        // print the names
        for (int i = 0; i < count; i++)
        {
            printf("%s\n", files[i]);
        }
    }

    else if (strcmp(args[1], "-l") == 0 && args[2][0] == '\0')
    {
        DIR *dr = opendir(".");

        if (dr == NULL) // opendir returns NULL if couldn't open directory
        {
            printf("Could not open directory");
            return;
        }

        struct stat info;

        while ((de = readdir(dr)) != NULL) // returns pointer to next element in the directory stream
        {
            char path[500] = "./";
            strcat(path, de->d_name);
            int val = lstat(path, &info);
            if (val == -1)
            {
                perror("lstat() error");
                return;
            }

            if (de->d_name[0] != '.')
            {
                printf("%s\t\t %d\t\t %ld\n", de->d_name, info.st_mode, info.st_size);
            }
        }
    }

    return;
}