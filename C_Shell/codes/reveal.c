#include "reveal.h"

int compare_reveal(const void *a, const void *b)
{
    struct dirent *entryA = *(struct dirent **)a;
    struct dirent *entryB = *(struct dirent **)b;
    const char *nameA = entryA->d_name;
    const char *nameB = entryB->d_name;
    if (entryA->d_name[0] == '.')
    {
        nameA = entryA->d_name + 1;
    }
    if (entryB->d_name[0] == '.')
    {
        nameB = entryB->d_name + 1;
    }
    return strcmp(nameA, nameB);
}

void print_permissions(struct stat file_stat)
{
    if (S_ISDIR(file_stat.st_mode))
    {
        printf("d");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IRUSR)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IWUSR)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IXUSR)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IRGRP)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IWGRP)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IXGRP)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IROTH)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IWOTH)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }

    if (file_stat.st_mode & S_IXOTH)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }
    return;
}

char *colour(struct stat file_stat)
{
    if (S_ISDIR(file_stat.st_mode))
    {
        return COLOR_BLUE;
    }
    if (file_stat.st_mode & S_IXUSR)
    {
        return COLOR_GREEN;
    }
    return COLOR_WHITE;
}

int get_path(char *path, char *current_dir, char *old_dir, char *home_dir)
{
    char temp[MAX_LENGTH];
    if (!strcmp(path, "~"))
    {
        strcpy(path, home_dir);
        return 0;
    }
    if (!strcmp(path, ".."))
    {
        strcpy(path, current_dir);
        if (access("..", X_OK))
        {
            printf("Missing permissions for task!\n");
            return 1;
        }
        if (chdir("..") != 0)
        {
            perror("Unable to change directory");
            return 1;
        }
        getcwd(path, MAX_LENGTH);
        if (chdir(current_dir) != 0)
        {
            perror("Unable to change directory");
            return 1;
        }
        return 0;
    }
    if (!strcmp(path, "-"))
    {
        if (!strcmp(old_dir, ""))
        {
            perror("No previous directory");
            return 1;
        }
        strcpy(path, old_dir);
        return 0;
    }
    if (!strcmp(path, "."))
    {
        strcpy(path, current_dir);
        return 0;
    }
    else
    {
        if (path[0] == '~')
        {
            strcpy(temp, home_dir);
            strcat(temp, path + 1);
            strcpy(path, temp);
            return 0;
        }
        if (path[0] == '/')
        {
            return 0;
        }
        if (path[0] == '.')
        {
            strcpy(temp, current_dir);
            strcat(temp, path + 1);
            strcpy(path, temp);
            return 0;
        }
        strcpy(temp, current_dir);
        strcat(temp, "/");
        strcat(temp, path);
        strcpy(path, temp);
        return 0;
    }
}

int all_details(char *current_dir, char *filepath, char *filename, bool skip)
{
    struct stat file_stat;
    if (stat(filepath, &file_stat) == -1)
    {
        perror("Unable to get file stat");
        return 1;
    }
    char *color = colour(file_stat);
    print_permissions(file_stat);
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group *gr = getgrgid(file_stat.st_gid);
    char time[80];
    char buffer[MAX_LENGTH];
    strftime(time, sizeof(time), "%b %d %H:%M", localtime(&file_stat.st_mtime));
    printf(" %lu", file_stat.st_nlink);
    printf(" %s %s", pw->pw_name, gr->gr_name);
    printf(" %ld", file_stat.st_size);
    printf(" %s", time);

    if (skip)
    {
        if (isatty(STDOUT_FILENO))
        {
            printf(" %s%s%s\n", color, filename + strlen(current_dir) + 1, COLOR_RESET);
        }
        else
        {
            printf(" %s\n", filename + strlen(current_dir) + 1);
        }
    }
    else
    {
        if (isatty(STDOUT_FILENO))
        {
            printf(" %s%s%s\n", color, filename, COLOR_RESET);
        }
        else
        {
            printf(" %s\n", filename);
        }
    }
    return 0;
}

int reveal(char *current_dir, char *path, bool a, bool l)
{
    DIR *dir;
    struct dirent *entry;
    struct stat path_stat;
    if (stat(path, &path_stat) == -1)
    {
        perror("Unable to get file stat");
        return 1;
    }
    if (S_ISREG(path_stat.st_mode))
    {
        if (l)
        {
            all_details(current_dir, path, path, true);
        }
        else
        {
            char *color = colour(path_stat);
            if (isatty(STDOUT_FILENO))
            {
                printf(" %s%s%s\n", color, path + strlen(current_dir) + 1, COLOR_RESET);
            }
            else
            {
                printf(" %s\n", path + strlen(current_dir) + 1);
            }
        }
        return 0;
    }
    if ((dir = opendir(path)) == NULL)
    {
        perror("Unable to open directory");
        return 1;
    }
    int count = 0;
    while ((entry = readdir(dir)))
    {
        count++;
    }
    rewinddir(dir);
    struct dirent **files = malloc(count * sizeof(struct dirent *));
    if (!files)
    {
        perror("Unable to allocate space");
        closedir(dir);
        free(files);
        return 1;
    }
    int index = 0;
    while ((entry = readdir(dir)))
    {
        files[index] = entry;
        index++;
    }
    int total = 0;
    qsort(files, count, sizeof(struct dirent *), compare_reveal);
    for (int i = 0; i < count; i++)
    {
        if (!a && files[i]->d_name[0] == '.')
        {
            continue;
        }
        char filepath[MAX_LENGTH];
        struct stat file_stat;
        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, files[i]->d_name);
        if (stat(filepath, &file_stat) == -1)
        {
            perror("Unable to get file stat");
            return 1;
        }
        total += file_stat.st_blocks;
    }
    char buffer[MAX_LENGTH];
    if (l)
    {
        printf("total %d\n", total / 2);
    }
    for (int i = 0; i < count; i++)
    {
        if (!a && files[i]->d_name[0] == '.')
        {
            continue;
        }
        char filepath[MAX_LENGTH];
        struct stat file_stat;
        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, files[i]->d_name);
        if (stat(filepath, &file_stat) == -1)
        {
            perror("Unable to get file stat");
            return 1;
        }
        if (l)
        {
            all_details(current_dir, filepath, files[i]->d_name, false);
        }
        else
        {
            char *color = colour(file_stat);
            if (isatty(STDOUT_FILENO))
            {
                printf(" %s%s%s\n", color, files[i]->d_name, COLOR_RESET);
            }
            else
            {
                printf(" %s\n", files[i]->d_name);
            }
        }
    }
    return 0;
}

int reveal_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    char path[MAX_LENGTH] = ".";
    bool a = false;
    bool l = false;
    if (counter == 1)
    {
        if (get_path(path, current_dir, old_dir, home_dir))
        {
            return 1;
        }
        reveal(current_dir, path, a, l);
        gettimeofday(&end, NULL);
        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        if (time_taken >= 2)
        {
            strcpy(command, commands[0]);
            *time = time_taken;
        }
        return 0;
    }
    for (int i = 1; i < counter - 1; i++)
    {
        for (unsigned j = 1; j < strlen(commands[i]); j++)
        {
            if (commands[i][j] == 'a')
            {
                a = true;
            }
            if (commands[i][j] == 'l')
            {
                l = true;
            }
        }
    }
    if (commands[counter - 1][0] == '-')
    {
        if (strlen(commands[counter - 1]) == 1)
        {
            strcpy(path, commands[counter - 1]);
        }
        else
        {
            for (unsigned j = 1; j < strlen(commands[counter - 1]); j++)
            {
                if (commands[counter - 1][j] == 'a')
                {
                    a = true;
                }
                if (commands[counter - 1][j] == 'l')
                {
                    l = true;
                }
            }
        }
    }
    else
    {
        strcpy(path, commands[counter - 1]);
    }
    if (get_path(path, current_dir, old_dir, home_dir))
    {
        return 1;
    }
    if (reveal(current_dir, path, a, l))
    {
        return 1;
    }
    gettimeofday(&end, NULL);
    int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    if (time_taken >= 2)
    {
        strcpy(command, commands[0]);
        *time = time_taken;
    }
    return 0;
}