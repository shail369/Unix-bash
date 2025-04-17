#include "proclore.h"
#include "reveal.h"

int proclore(char *home_dir, pid_t pid)
{
    char path[MAX_LENGTH];
    char execute[MAX_LENGTH];
    char state[3];
    state[2] = '\0';
    state[1] = '\0';
    FILE *file;
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Failed to open /proc/<pid>/status");
        return 1;
    }
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            state[0] = line[7];
            break;
        }
    }
    fclose(file);

    pid_t pgid = getpgid(pid);
    pid_t tty_pgid = tcgetpgrp(STDIN_FILENO);
    if (pgid == tty_pgid)
    {
        state[1] = '+';
    }

    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Failed to open /proc/<pid>/statm");
        return 1;
    }
    long int virtual_memory;
    fscanf(file, "%ld", &virtual_memory);
    fclose(file);
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    int len = readlink(path, execute, sizeof(execute) - 1);
    if (len != -1)
    {
        execute[len] = '\0';
    }
    else
    {
        strcpy(execute, " ");
    }
    char buffer[MAX_LENGTH];
    printf("pid : %d\n", pid);
    printf("Process Status: %s\n", state);
    printf("Process Group: %d\n", pgid);
    printf("Virtual Memory: %ld KB\n", virtual_memory * 4);
    if (strncmp(execute, home_dir, strlen(home_dir)) == 0)
    {
        printf("Executable Path: %s\n", execute + strlen(home_dir));
    }
    else
    {
        printf("Executable Path: %s\n", execute);
    }
    return 0;
}

int proclore_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    if (counter == 1)
    {
        pid_t pid = getpid();
        if (proclore(home_dir, pid))
        {
            return 1;
        }
    }
    else
    {
        pid_t pid = atoi(commands[1]);
        if (proclore(home_dir, pid))
        {
            return 1;
        }
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