#include "hop.h"

int hop(char *home_dir, char *current_dir, char *old_dir, char *directory, char *new_dir)
{
    if (!strcmp(directory, "~"))
    {
        strcpy(new_dir, home_dir);
        return 0;
    }
    if (!strcmp(directory, ".."))
    {
        strcpy(new_dir, current_dir);
        if (access("..", X_OK))
        {
            perror("Missing permissions for task");
            return 1;
        }
        if (chdir("..") != 0)
        {
            perror("Unable to change directory");
            return 1;
        }
        getcwd(new_dir, MAX_LENGTH);
        return 0;
    }
    if (!strcmp(directory, "-"))
    {
        if (!strcmp(old_dir, ""))
        {
            perror("No previous directory");
            return 1;
        }
        strcpy(new_dir, old_dir);
        return 0;
    }
    if (!strcmp(directory, "."))
    {
        strcpy(new_dir, current_dir);
        return 0;
    }
    else
    {
        if (directory[0] == '~')
        {
            strcpy(new_dir, home_dir);
            strcat(new_dir, directory + 1);
            return 0;
        }
        if (directory[0] == '/')
        {
            strcpy(new_dir, directory);
            return 0;
        }
        if (directory[0] == '.')
        {
            strcpy(new_dir, current_dir);
            strcat(new_dir, directory + 1);
            return 0;
        }
        strcpy(new_dir, current_dir);
        strcat(new_dir, "/");
        strcat(new_dir, directory);
        return 0;
    }
}

int hop_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time )
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    char new_dir[MAX_LENGTH];
    char *home = home_dir;
    if (counter == 1)
    {
        if (chdir(home_dir) != 0)
        {
            perror("Unable to change directory");
            return 1;
        }
        strcpy(old_dir, current_dir);
        chdir(home_dir);
        strcpy(current_dir, home_dir);

        printf("%s\n", current_dir);
        gettimeofday(&end, NULL);
        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        if (time_taken >= 2)
        {
            strcpy(command, commands[0]);
            *time = time_taken;
        }
        return 0;
    }
    for (int i = 1; i < counter; i++)
    {
        if (hop(home, current_dir, old_dir, commands[i], new_dir))
        {
            return 1;
        }
        if (access(new_dir, X_OK))
        {
            printf("Missing permissions for task!\n");
            return 1;
        }
        if (chdir(new_dir) != 0)
        {
            perror("Unable to change directory");
            return 1;
        }
        strcpy(old_dir, current_dir);
        strcpy(current_dir, new_dir);
        printf("%s\n", current_dir);
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
