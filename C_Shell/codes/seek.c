#include "seek.h"
#include "reveal.h"

int seek(char *current_dir, char *reference, char *dir_name, char *target, bool d, bool f, bool e, char *next_dir, char *file, int *dir_count, int *file_count)
{
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(dir_name)) == NULL)
    {
        perror("Error opening directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
        {
            continue;
        }
        char path[MAX_LENGTH];
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);
        struct stat path_stat;
        char buffer[MAX_LENGTH];
        if (stat(path, &path_stat) == 0)
        {
            if (strncmp(entry->d_name, target, strlen(target)) == 0)
            {
                if (S_ISDIR(path_stat.st_mode))
                {
                    if (!f)
                    {
                        (*dir_count)++;
                        if (isatty(STDOUT_FILENO))
                        {
                            printf(COLOR_BLUE ".%s/%s\n" COLOR_RESET, dir_name + strlen(reference), entry->d_name);
                        }
                        else
                        {
                            printf(".%s/%s\n", dir_name + strlen(reference), entry->d_name);
                        }
                        strcpy(next_dir, dir_name);
                        strcat(next_dir, "/");
                        strcat(next_dir, entry->d_name);
                    }
                }
                else
                {
                    if (!d)
                    {
                        (*file_count)++;
                        if (isatty(STDOUT_FILENO))
                        {
                            printf(COLOR_GREEN ".%s/%s\n" COLOR_RESET, dir_name + strlen(reference), entry->d_name);
                        }
                        else
                        {
                            printf(".%s/%s\n", dir_name + strlen(reference), entry->d_name);
                        }
                        strcpy(file, dir_name);
                        strcat(file, "/");
                        strcat(file, entry->d_name);
                    }
                }
            }
            if (S_ISDIR(path_stat.st_mode))
            {
                seek(current_dir, reference, path, target, d, f, e, next_dir, file, dir_count, file_count);
            }
        }
        else
        {
            continue;
        }
    }
    closedir(dir);
    return 0;
}

int special(char *home_dir, char *current_dir, char *old_dir, char *next_dir, char *file, int dir_count, int file_count, bool e)
{
    if (dir_count == 1 && e && !file_count)
    {
        if (access(next_dir, X_OK))
        {
            perror("Missing permissions for task");
            return 1;
        }
        if (chdir(next_dir) == -1)
        {
            perror("Unable to change directory");
            return 1;
        }
        strcpy(old_dir, current_dir);
        getcwd(current_dir, sizeof(current_dir));
        strcpy(current_dir, next_dir);
    }
    if (file_count == 1 && e && !dir_count)
    {
        struct stat file_stat;

        if (stat(file, &file_stat) == -1)
        {
            perror("Error getting file stat");
            return 1;
        }
        if (file_stat.st_mode & S_IXUSR)
        {
            fflush(stdout);
            char *args[] = {file, NULL};
            execvp(file, args);
            perror("Error executing file");
            return 1;
        }
        else
        {
            if (access(file, R_OK))
            {
                printf("Missing permissions for task!\n");
                return 1;
            }
            FILE *print = fopen(file, "r");
            if (print == NULL)
            {
                perror("Error opening file");
                return 1;
            }
            int ch;
            while ((ch = fgetc(print)) != EOF)
            {
                putchar(ch);
            }
            fclose(print);
        }
        fflush(stdout);
        fflush(stdin);
        return 0;
    }
    if (!dir_count && !file_count)
    {
        printf("No Match Found\n");
    }
    return 0;
}

int seek_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    bool d = false;
    bool f = false;
    bool e = false;
    char target[MAX_LENGTH];
    char directory[MAX_LENGTH];
    if (counter < 2)
    {
        printf("Invalid arguments\n");
        return 1;
    }
    for (int i = 1; i < counter - 2; i++)
    {
        if (commands[i][0] == '-')
        {
            for (unsigned j = 1; j < strlen(commands[i]); j++)
            {
                if (commands[i][j] == 'd')
                {
                    d = true;
                }
                if (commands[i][j] == 'f')
                {
                    f = true;
                }
                if (commands[i][j] == 'e')
                {
                    e = true;
                }
            }
        }
    }
    if (d && f)
    {
        printf("Invalid flags\n");
        return 1;
    }
    if (counter == 2)
    {
        strcpy(directory, current_dir);
        strcpy(target, commands[counter - 1]);
    }
    else if (commands[counter - 2][0] == '-')
    {
        for (unsigned j = 1; j < strlen(commands[counter - 2]); j++)
        {
            if (commands[counter - 2][j] == 'd')
            {
                d = true;
            }
            if (commands[counter - 2][j] == 'f')
            {
                f = true;
            }
            if (commands[counter - 2][j] == 'e')
            {
                e = true;
            }
        }
        if (d && f)
        {
            printf("Invalid flags\n");
            return 1;
        }
        strcpy(target, commands[counter - 1]);
        strcpy(directory, current_dir);
    }
    else
    {
        get_path(commands[counter - 1], current_dir, old_dir, home_dir);
        strcpy(target, commands[counter - 2]);
        strcpy(directory, commands[counter - 1]);
    }
    char next_dir[MAX_LENGTH];
    next_dir[0] = '\0';
    char file[MAX_LENGTH];
    file[0] = '\0';
    int dir_count = 0;
    int file_count = 0;
    directory[strlen(directory)] = '\0';
    if (seek(current_dir, directory, directory, target, d, f, e, next_dir, file, &dir_count, &file_count))
    {
        return 1;
    }
    fflush(stdout);
    if (special(home_dir, current_dir, old_dir, next_dir, file, dir_count, file_count, e))
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
