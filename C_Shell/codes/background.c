#include "background.h"
#include "helper.h"
#include "activities.h"
#include <signal.h>
#include <sys/wait.h>

void handle_sigchld(int sig)
{
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (processes[i].pid == pid)
            {
                if (WIFEXITED(status))
                {
                    remove_process(pid);
                    printf("%s exited normally (%d)\n", processes[i].cmd_name, pid);
                }
                else if (WIFSIGNALED(status))
                {
                    remove_process(pid);
                    printf("%s exited abnormally (%d)\n", processes[i].cmd_name, pid);
                }
                else if (WIFSTOPPED(status))
                {
                    stop_process(pid);
                    printf("%s was stopped (%d)\n", processes[i].cmd_name, pid);
                }
                break;
            }
        }
    }
}

void background_process(char *commands[], int counter)
{
    int pipe_pid[2], pipe_name[2];
    pid_t pid_1, pid_2;
    int pid;
    char process_name[MAX_LENGTH];

    if (pipe(pipe_pid) == -1 || pipe(pipe_name) == -1)
    {
        perror("Pipe failed");
        return;
    }

    pid_1 = fork();
    if (pid_1 < 0)
    {
        perror("Failed to fork child");
        return;
    }

    if (pid_1 == 0)
    {
        setpgid(0, 0);
        pid_2 = fork();
        if (pid_2 < 0)
        {
            perror("Failed to fork child");
            return;
        }
        if (pid_2 == 0)
        {
            char *background_command[counter + 1];
            for (int j = 0; j < counter; j++)
            {
                background_command[j] = commands[j];
            }
            background_command[counter] = NULL;

            pid_2 = getpid();
            close(pipe_pid[0]);
            close(pipe_name[0]);
            write(pipe_name[1], background_command[0], strlen(background_command[0]) + 1);
            write(pipe_pid[1], &pid_2, sizeof(pid_2));
            close(pipe_name[1]);
            close(pipe_pid[1]);

            if (execvp(background_command[0], background_command) == -1)
            {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("(%d)\n", pid_2);
            int status;
            waitpid(pid_2, &status, 0);
            if (WIFEXITED(status))
            {
                printf("%s exited normally (%d)\n", commands[0], pid_2);
            }
            else if (WIFSIGNALED(status))
            {
                printf("%s exited abnormally (%d)\n", commands[0], pid_2);
            }
            else if (WIFSTOPPED(status))
            {
                printf("%s was stopped (%d)\n", commands[0], pid_2);
            }
            exit(EXIT_SUCCESS);
        }
    }
    else
    {
        dup2(stdin_fd, STDIN_FILENO);
        dup2(stdout_fd, STDOUT_FILENO);
        close(pipe_pid[1]);
        close(pipe_name[1]);

        read(pipe_pid[0], &pid, sizeof(pid));
        read(pipe_name[0], process_name, sizeof(process_name));
        close(pipe_pid[0]);
        close(pipe_name[0]);
        add_process(pid, process_name);
    }
    return;
}
