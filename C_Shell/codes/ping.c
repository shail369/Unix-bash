#include "ping.h"

void ctrl_c(int sig)
{
    if (foreground_pid != -1)
    {
        kill(foreground_pid, SIGINT);
    }
}

void ctrl_d()
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid)
        {
            kill(processes[i].pid, SIGKILL);
            processes[i].pid = 0;
        }
    }
    exit(0);
}

void ctrl_z(int sig)
{
    if (foreground_pid != -1)
    {
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processes[i].pid == foreground_pid)
            {
                strcpy(processes[i].state, "Stopped");
                break;
            }
        }
        setpgid(foreground_pid, foreground_pid);
        kill(foreground_pid, SIGTSTP);
        tcsetpgrp(STDIN_FILENO, getpid());
        foreground_pid = -1;
    }
    printf("\n");
}

int ping_main(int counter, char *commands[], char *command, int *time)
{
    if (counter != 3)
    {
        printf("Invalid Command\n");
        return 1;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    pid_t pid = atoi(commands[1]);
    int signal_number = atoi(commands[2]);
    signal_number %= 32;
    if (kill(pid, 0) == -1)
    {
        if (errno == ESRCH)
        {
            printf("No such process found\n");
        }
        else
        {
            perror("Error");
        }
        perror("Error sending signal");
        gettimeofday(&end, NULL);
        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        if (time_taken >= 2)
        {
            strcpy(command, "activities");
            *time = time_taken;
        }
        return 1;
    }
    if (kill(pid, signal_number) == -1)
    {
        perror("Error sending signal");
        gettimeofday(&end, NULL);
        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        if (time_taken >= 2)
        {
            strcpy(command, "activities");
            *time = time_taken;
        }
        return 1;
    }
    return 0;
}
