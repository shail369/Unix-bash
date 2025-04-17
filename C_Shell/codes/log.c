#include "log.h"

int existing(char *current[], char *current_dir, char *home_dir)
{
    chdir(home_dir);
    FILE *file = fopen("history", "a+");
    int index = 0;
    if (file)
    {
        char temp[MAX_LENGTH];
        while (fgets(temp, MAX_LENGTH, file))
        {
            temp[strcspn(temp, "\n")] = '\0';
            if (strlen(temp) == 0)
            {
                continue;
            }
            strcpy(current[index], temp);
            index++;
        }
        fclose(file);
        chdir(current_dir);
        return index;
    }
    perror("Error opening history file");
    chdir(current_dir);
    return -1;
}

int store(char *command, char *current_dir, char *home_dir)
{
    char *current[15];
    for (int i = 0; i < 15; i++)
    {
        current[i] = malloc(MAX_LENGTH);
    }
    chdir(home_dir);
    int curr_num = existing(current, current_dir, home_dir);
    if (curr_num < 0)
    {
        for (int i = 0; i < 15; i++)
        {
            free(current[i]);
        }
        return 1;
    }
    if (curr_num && !strcmp(command, current[curr_num - 1]))
    {
        chdir(current_dir);
        for (int i = 0; i < 15; i++)
        {
            free(current[i]);
        }
        return 0;
    }
    chdir(home_dir);
    FILE *file = fopen("history", "w");
    if (file)
    {
        if (curr_num == 15)
        {
            for (int i = 1; i < curr_num; i++)
            {
                fprintf(file, "%s\n", current[i]);
            }
            fprintf(file, "%s\n", command);
            fclose(file);
            chdir(current_dir);
            for (int i = 0; i < 15; i++)
            {
                free(current[i]);
            }
            return 0;
        }
        for (int i = 0; i < curr_num; i++)
        {
            fprintf(file, "%s\n", current[i]);
        }
        fprintf(file, "%s\n", command);
        fclose(file);
        chdir(current_dir);
        for (int i = 0; i < 15; i++)
        {
            free(current[i]);
        }
        return 0;
    }
    perror("Error opening history file for writing");
    chdir(current_dir);
    for (int i = 0; i < 15; i++)
    {
        free(current[i]);
    }
    return 1;
}

int history(char *current_dir, char *home_dir)
{
    chdir(home_dir);
    FILE *file = fopen("history", "a+");
    if (file)
    {
        char temp[MAX_LENGTH];
        while (fgets(temp, MAX_LENGTH, file))
        {
            temp[strcspn(temp, "\n")] = '\0';
            printf("%s\n", temp);
        }
        fclose(file);
        chdir(current_dir);
        return 0;
    }
    perror("Error opening history file");
    chdir(current_dir);
    return 1;
}

int clear(char *current_dir, char *home_dir)
{
    chdir(home_dir);
    FILE *file = fopen("history", "w");
    if (!file)
    {
        perror("Error opening history file for clearing");
        chdir(current_dir);
        return 1;
    }
    fclose(file);
    chdir(current_dir);
    return 0;
}

int execute(int index, char *current_dir, char *home_dir, char *old_dir, char *commands_print, int *time)
{
    chdir(home_dir);
    FILE *file = fopen("history", "a+");
    int total_number = 0;
    if (file)
    {
        char temp[MAX_LENGTH];
        while (fgets(temp, MAX_LENGTH, file))
        {
            temp[strcspn(temp, "\n")] = '\0';
            total_number++;
        }
        fclose(file);
        FILE *file = fopen("history", "a+");
        char command[MAX_LENGTH];
        if (index > total_number)
        {
            printf("Invalid command\n");
            chdir(current_dir);
            return 1;
        }
        for (int i = 0; i < total_number - index; i++)
        {
            fgets(temp, MAX_LENGTH, file);
        }
        fgets(command, MAX_LENGTH, file);
        command[strcspn(command, "\n")] = '\0';
        chdir(current_dir);
        if (solve(command, current_dir, home_dir, old_dir, commands_print, time))
        {
            return 1;
        }
        return 0;
    }
    perror("Error opening history file");
    chdir(current_dir);
    return 1;
}

int log_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    if (counter > 3)
    {
        printf("Invalid command\n");
        return 1;
    }
    if (counter == 1)
    {
        if (history(current_dir, home_dir))
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
    if (!strcmp(commands[1], "purge"))
    {
        if (clear(current_dir, home_dir))
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
    else if (!strcmp(commands[1], "execute"))
    {
        int index = atoi(commands[2]);
        if (index > 0 && index <= 15)
        {
            if (execute(index, current_dir, home_dir, old_dir, command, time))
            {
                return 1;
            }
        }
        else
        {
            printf("Invalid command\n");
            return 1;
        }
        return 0;
    }
    else
    {
        printf("Invalid command\n");
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