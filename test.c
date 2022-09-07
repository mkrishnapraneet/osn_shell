#include <stdio.h>
#include <dirent.h>
  
int main(void)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char path[500];
    char *file_type;
    char *file_permission;
    char *file_owner;
    char *file_group;
    char *file_size;
    char *file_time;
    char *file_name;
    char *file_link;
    char *file_inode;
    char *file_blocks;
    char *file_device;
    char *file_rdev;
    char *file_blksize;

    if (!(dir = opendir(target_dir)))
        return;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            // skip the current and parent directory
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            // construct the path of the subdirectory
            snprintf(path, sizeof(path), "%s/%s", target_dir, entry->d_name);
            // recursively list all components in the subdirectory
            list_all_components(path);
        }
        else
        {
            // construct the path of the file
            snprintf(path, sizeof(path), "%s/%s", target_dir, entry->d_name);
            // get the file status
            if (stat(path, &file_stat) == -1)
            {
                printf("Error: %s");
            }
        }
    }
}