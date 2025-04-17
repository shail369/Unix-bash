#include "fgbg.h"

int fg_command(pid_t pid, int shell_pgid)
{
    int found = 0;
    int status;
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid)
        {
            found = 1;
            if (tcsetpgrp(STDIN_FILENO, pid) == -1)
            {
                signal(SIGTTOU, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                perror("Failed to bring process to background");
                return 1;
            }
            if (!strcmp(processes[i].state, "Stopped"))
            {
                if (kill(pid, SIGCONT) == -1)
                {
                    signal(SIGTTOU, SIG_DFL);
                    signal(SIGTTIN, SIG_DFL);
                    perror("Failed to send SIGCONT");
                    return 1;
                }
            }
            if (waitpid(pid, &status, WUNTRACED) == -1)
            {
                signal(SIGTTOU, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                perror("waitpid failed");
                return 1;
            }

            if (WIFSTOPPED(status))
            {
                strcpy(processes[i].state, "Stopped");
                printf("%s (PID: %d) was stopped\n", processes[i].cmd_name, pid);
            }
            else
            {
                processes[i].pid = 0;
                printf("%s (PID: %d) has finished\n", processes[i].cmd_name, pid);
            }

            if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1)
            {
                signal(SIGTTOU, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                perror("Failed to return control to terminal");
                return 1;
            }
            break;
        }
    }
    if (!found)
    {
        printf("No such process found\n");
    }
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    return 0;
}

int bg_command(pid_t pid)
{
    int found = 0;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid)
        {
            found = 1;
            pid_t pgid = getpgid(pid);
            pid_t tty_pgid = tcgetpgrp(STDIN_FILENO);
            if (!strcmp(processes[i].state, "Stopped") && pgid != tty_pgid)
            {
                if (kill(pid, SIGCONT) == 0)
                {
                    strcpy(processes[i].state, "Running");
                    printf("%s (PID: %d) is now running in the background\n", processes[i].cmd_name, pid);
                    return 0;
                }
                else
                {
                    perror("Failed to send SIGCONT");
                    return 1;
                }
            }
            else
            {
                printf("%s (PID: %d) is already running\n", processes[i].cmd_name, pid);
                return 1;
            }
            break;
        }
    }

    if (!found)
    {
        printf("No such process found\n");
        return 0;
    }
}

int command_main(int counter, char *commands[], char *command, int *time)
{
    if (counter != 2)
    {
        printf("Invalid Arguments\n");
        return 1;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    pid_t pid = atoi(commands[1]);
    if (!strcmp(commands[0], "bg"))
    {
        if (bg_command(pid))
        {
            gettimeofday(&end, NULL);
            int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
            if (time_taken >= 2)
            {
                strcpy(command, "activities");
                *time = time_taken;
            }
            return 1;
        }
    }
    else
    {
        int shell_pgid = getpid();
        if (fg_command(pid, shell_pgid))
        {
            gettimeofday(&end, NULL);
            int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
            if (time_taken >= 2)
            {
                strcpy(command, "activities");
                *time = time_taken;
            }
            return 1;
        }
    }
    gettimeofday(&end, NULL);
    int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    if (time_taken >= 2)
    {
        strcpy(command, "activities");
        *time = time_taken;
    }
    return 0;
}