#include "helper.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "background.h"
#include "proclore.h"
#include "seek.h"
#include "myshrc.h"
#include "IOredirection.h"
#include "activities.h"
#include "ping.h"
#include "fgbg.h"
#include "neonate.h"
#include "iman.h"

int foreground_pid = -1;

void get_details(char *username, char *systemname)
{
    struct passwd *pw;
    struct utsname uname_data;
    pw = getpwuid(getuid());
    uname(&uname_data);
    strcpy(username, pw->pw_name);
    strcpy(systemname, uname_data.nodename);
    return;
}

void get_prompt(char *current_dir, char *home_dir, char *username, char *systemname, char *command, int time)
{
    if (command[0] != '\0')
    {
        if (strncmp(current_dir, home_dir, strlen(home_dir)) == 0)
        {
            printf("<%s@%s:~%s %s : %ds> ", username, systemname, current_dir + strlen(home_dir), command, time);
        }
        else
        {
            printf("<%s@%s:%s %s : %ds> ", username, systemname, current_dir, command, time);
        }
    }
    else
    {
        if (strncmp(current_dir, home_dir, strlen(home_dir)) == 0)
        {
            printf("<%s@%s:~%s> ", username, systemname, current_dir + strlen(home_dir));
        }
        else
        {
            printf("<%s@%s:%s> ", username, systemname, current_dir);
        }
    }
    return;
}

int multiple(char *original_command, int counter_and[], int counter_individual[], int counter_pipe[], char *individual_commands[])
{
    char *semicolon_commands[MAX_LENGTH];
    char *pipe_commands[MAX_LENGTH];
    char *and_commands[MAX_LENGTH];
    int num_semicolon_commands = 0;
    int num_pipe_commands = 0;
    int num_and_commands = 0;
    int num_individual_commands = 0;
    char *temp_semi = strtok(original_command, ";");
    while (temp_semi)
    {
        semicolon_commands[num_semicolon_commands] = malloc(strlen(temp_semi) + 3);
        strcpy(semicolon_commands[num_semicolon_commands], " ");
        strcat(semicolon_commands[num_semicolon_commands], temp_semi);
        strcat(semicolon_commands[num_semicolon_commands++], " ");
        temp_semi = strtok(NULL, ";");
    }
    semicolon_commands[num_semicolon_commands] = NULL;
    for (int i = 0; i < num_semicolon_commands; i++)
    {
        counter_pipe[i] = 0;
    }
    for (int i = 0; i < num_semicolon_commands; i++)
    {
        char *temp_pipe = strtok(semicolon_commands[i], "|");
        while (temp_pipe)
        {
            pipe_commands[num_pipe_commands] = malloc(strlen(temp_pipe) + 3);
            strcpy(pipe_commands[num_pipe_commands], " ");
            strcat(pipe_commands[num_pipe_commands], temp_pipe);
            strcat(pipe_commands[num_pipe_commands++], " ");
            counter_pipe[i]++;
            temp_pipe = strtok(NULL, "|");
        }
    }
    pipe_commands[num_pipe_commands] = NULL;
    for (int i = 0; i < num_pipe_commands; i++)
    {
        counter_and[i] = 0;
    }
    for (int i = 0; i < num_pipe_commands; i++)
    {
        char *temp_and = strtok(pipe_commands[i], "&");
        while (temp_and)
        {
            and_commands[num_and_commands] = malloc(strlen(temp_and) + 3);
            strcpy(and_commands[num_and_commands++], temp_and);
            counter_and[i]++;
            temp_and = strtok(NULL, "&");
        }
    }
    and_commands[num_and_commands] = NULL;
    for (int i = 0; i < num_and_commands; i++)
    {
        counter_individual[i] = 0;
    }
    for (int i = 0; i < num_and_commands; i++)
    {
        int skip = 1;
        for (unsigned j = 0; j < strlen(and_commands[i]); j++)
        {
            if (and_commands[i][j] != ' ')
            {
                skip = 0;
                break;
            }
        }
        if (skip)
        {
            strcpy(individual_commands[num_individual_commands++], " ");
            continue;
        }
        char *temp_individual = strtok(and_commands[i], " ");
        while (temp_individual)
        {
            strcpy(individual_commands[num_individual_commands++], temp_individual);
            counter_individual[i]++;
            temp_individual = strtok(NULL, " ");
        }
    }
    individual_commands[num_individual_commands] = NULL;
    for (int i = 0; i < num_semicolon_commands; i++)
    {
        free(semicolon_commands[i]);
    }
    for (int i = 0; i < num_pipe_commands; i++)
    {
        free(pipe_commands[i]);
    }
    for (int i = 0; i < num_and_commands; i++)
    {
        free(and_commands[i]);
    }
    return num_individual_commands;
}

void cleanup_variables(char *individual_commands[], int num_individual_commands)
{
    for (int i = 0; i < num_individual_commands; i++)
    {
        free(individual_commands[i]);
    }
}

int open_output_file(char *output_filename, int flags, int mode)
{
    int redirect_output = open(output_filename, flags, mode);
    if (redirect_output == -1)
    {
        perror("Error opening file");
    }
    return redirect_output;
}

int open_input_file(char *input_filename)
{
    FILE *file = freopen(input_filename, "r", stdin);
    if (file == NULL)
    {
        printf("No such input file found\n");
        return 1;
    }
    return 0;
}

int set_stdout(int redirect_output, bool condition)
{
    if (condition)
    {
        if (dup2(redirect_output, STDOUT_FILENO) == -1)
        {
            perror("Error redirecting stdout");
            return 1;
        }
        close(redirect_output);
    }
    return 0;
}

int reset_stdout(int stdout_fd, bool condition)
{
    if (condition)
    {
        if (dup2(stdout_fd, STDOUT_FILENO) == -1)
        {
            perror("Error restoring stdout");
            return 1;
        }
    }
    return 0;
}

int reset_stdin(int stdin_fd, bool condition)
{
    if (condition)
    {
        if (dup2(stdin_fd, STDOUT_FILENO) == -1)
        {
            perror("Error restoring stdout");
            return 1;
        }
    }
    return 0;
}

bool is_background(int *counter, int *index_and, bool has_pipe)
{
    if (*counter > 1)
    {
        (*counter)--;
        if (has_pipe)
        {
            (*index_and)++;
        }
        return true;
    }
    else
    {
        (*index_and)++;
        return false;
    }
}

void next_iteration(int *i, int counter, int *index_individual, int num_individual_commands, char *commands[], char *individual_commands[])
{
    if (counter)
    {
        *i += counter - 1;
    }
    (*index_individual)++;
    cleanup_variables(commands, MAX_LENGTH);
    if (*i == num_individual_commands - 1)
    {
        cleanup_variables(individual_commands, MAX_LENGTH);
    }
}

int solve(char *original_command, char *current_dir, char *home_dir, char *old_dir, char *command, int *time)
{
    char log_command[MAX_LENGTH];
    strcpy(log_command, original_command);
    bool log_present = false;
    char *individual_commands[MAX_LENGTH];
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        individual_commands[i] = malloc(MAX_LENGTH);
    }
    int num_individual_commands = 0;
    int counter_and[MAX_LENGTH];
    int counter_individual[MAX_LENGTH];
    int counter_pipe[MAX_LENGTH];

    num_individual_commands = multiple(original_command, counter_and, counter_individual, counter_pipe, individual_commands);

    int index_and = 0;
    int index_individual = 0;
    int index_pipe = 0;

    int num_of_pipe = 0;
    int temp_index = 0;

    for (int i = 0; i < num_individual_commands; i++)
    {
        if (counter_individual[temp_index])
        {
            i += counter_individual[temp_index] - 1;
            num_of_pipe++;
        }
    }

    int pipe_fds[num_of_pipe][2];
    int pipe_index = 0;
    bool has_pipe = false;
    bool prev_has_pipe = false;
    bool background = false;

    for (int i = 0; i < num_individual_commands; i++)
    {
        if (!strcmp(individual_commands[i], "log"))
        {
            log_present = true;
        }
        if (!strcmp(individual_commands[i], "exit"))
        {
            log_present = true;
        }
    }
    if (!log_present)
    {
        store(log_command, current_dir, home_dir);
    }
    for (int i = 0; i < num_individual_commands; i++)
    {
        stop = 0;
        if (!strcmp(individual_commands[i], "exit"))
        {
            cleanup_variables(individual_commands, MAX_LENGTH);
            return 2;
        }
        int new_count = 0;
        char *commands[MAX_LENGTH];
        for (int j = 0; j < MAX_LENGTH; j++)
        {
            commands[j] = (char *)malloc(sizeof(char) * MAX_LENGTH);
        }
        for (int j = 0; j < counter_individual[index_individual]; j++)
        {
            strcpy(commands[j], individual_commands[i + j]);
        }
        prev_has_pipe = has_pipe;
        if (counter_and[index_and] == 1 && counter_pipe[index_pipe] > 1)
        {
            has_pipe = true;
        }
        else if (counter_and[index_and] == 2 && i < num_individual_commands - 1 && !counter_individual[index_individual + 1] && counter_pipe[index_pipe] > 1)
        {
            has_pipe = true;
        }
        if (counter_and[index_and] == 2)
        {
            background = true;
        }

        if (!counter_individual[index_individual] && i == num_individual_commands - 1)
        {
            cleanup_variables(individual_commands, MAX_LENGTH);
            cleanup_variables(commands, MAX_LENGTH);
            return 1;
        }

        if (!counter_individual[index_individual] && (has_pipe || background))
        {
            printf("Invalid Input\n");
            cleanup_variables(individual_commands, MAX_LENGTH);
            cleanup_variables(commands, MAX_LENGTH);
            return 1;
        }
        else if (!counter_individual[index_individual])
        {
            next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
            continue;
        }

        has_pipe = false;
        bool next_and = false;
        if (counter_and[index_and] == 1 && counter_pipe[index_pipe] > 1)
        {
            counter_pipe[index_pipe]--;
            has_pipe = true;
        }
        else if (counter_and[index_and] == 1)
        {
            index_pipe++;
        }
        else if (counter_and[index_and] == 2 && i < num_individual_commands - 1 && !counter_individual[index_individual + 1] && counter_pipe[index_pipe] > 1)
        {
            counter_pipe[index_pipe]--;
            has_pipe = true;
            next_and = true;
        }
        else if (counter_and[index_and] == 2 && i < num_individual_commands - 1 && !counter_individual[index_individual + 1])
        {
            index_pipe++;
            next_and = true;
        }
        background = is_background(&counter_and[index_and], &index_and, next_and);
        if (background && has_pipe)
        {
            printf("Invalid Input\n");
            cleanup_variables(individual_commands, MAX_LENGTH);
            cleanup_variables(commands, MAX_LENGTH);
            return 1;
        }
        if (has_pipe && (i + counter_individual[index_individual] == num_individual_commands || !counter_individual[index_individual + 1]))
        {
            printf("Invalid Input\n");
            cleanup_variables(individual_commands, MAX_LENGTH);
            cleanup_variables(commands, MAX_LENGTH);
            return 1;
        }
        bool input_file = false;
        bool output_file_write = false;
        bool output_file_append = false;
        char input_filename[MAX_LENGTH];
        char output_filename[MAX_LENGTH];
        if (proper(commands, counter_individual[index_individual], &input_file, &output_file_write, &output_file_append, input_filename, output_filename))
        {
            next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
            continue;
        }
        for (int j = 0; j < counter_individual[index_individual]; j++)
        {
            if (!strcmp(individual_commands[i + j], ">") || !strcmp(individual_commands[i + j], ">>") || !strcmp(individual_commands[i + j], "<"))
            {
                j++;
            }
            else
            {
                strcpy(commands[new_count++], individual_commands[i + j]);
            }
        }
        for (int i = 0; i < MAX_PROCESSES; i++)
        {
            if (!strcmp(commands[0], alias[i]))
            {
                alias_replacement(i, commands, &new_count);
                break;
            }
        }
        if (has_pipe)
        {
            if (pipe(pipe_fds[pipe_index]) == -1)
            {
                perror("pipe");
                next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                continue;
            }

            pid_t pid_main = fork();
            if (pid_main == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid_main == 0)
            {
                if (has_pipe)
                {
                    close(pipe_fds[pipe_index][0]);
                    if (dup2(pipe_fds[pipe_index][1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(pipe_fds[pipe_index][1]);
                }

                if (prev_has_pipe)
                {
                    if (dup2(pipe_fds[pipe_index - 1][0], STDIN_FILENO) == -1)
                    {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(pipe_fds[pipe_index - 1][0]);
                }

                int redirect_output = 0;
                if (input_file)
                {
                    if (open_input_file(input_filename))
                    {
                        exit(EXIT_FAILURE);
                    }
                }
                if (output_file_write)
                {
                    redirect_output = open_output_file(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                else if (output_file_append)
                {
                    redirect_output = open_output_file(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                }
                if ((output_file_append || output_file_write) && !redirect_output)
                {
                    dup2(stdin_fd, STDOUT_FILENO);
                    exit(EXIT_FAILURE);
                }
                if (set_stdout(redirect_output, output_file_append || output_file_write))
                {
                    dup2(stdin_fd, STDOUT_FILENO);
                    exit(EXIT_FAILURE);
                }

                if (background)
                {
                    background_process(commands, new_count);
                    dup2(stdin_fd, STDIN_FILENO);
                    dup2(stdout_fd, STDOUT_FILENO);
                    exit(EXIT_SUCCESS);
                }
                if (!strcmp(commands[0], "hop"))
                {
                    hop_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "reveal"))
                {
                    reveal_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "log"))
                {
                    log_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "proclore"))
                {
                    proclore_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "seek"))
                {
                    seek_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "activities"))
                {
                    activities_main(new_count, command, time);
                }
                else if (!strcmp(commands[0], "ping"))
                {
                    ping_main(new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "bg") || !strcmp(individual_commands[i], "fg"))
                {
                    command_main(new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "neonate"))
                {
                    neonate_main(new_count, commands, command, time);
                }
                else if (!strcmp(commands[0], "iman"))
                {
                    iman_main(commands, new_count, command, time);
                }
                else if (!strcmp(commands[0], "exit"))
                {
                    for (int j = 0; j < MAX_LENGTH; j++)
                    {
                        free(individual_commands[j]);
                        free(commands[j]);
                    }
                    dup2(stdin_fd, STDIN_FILENO);
                    dup2(stdout_fd, STDOUT_FILENO);
                    return 2;
                }
                else
                {
                    struct timeval start, end;
                    gettimeofday(&start, NULL);
                    pid_t pid = fork();
                    if (pid == 0)
                    {
                        foreground_pid = getpid();
                        setpgid(0, 0);
                        commands[new_count] = NULL;
                        if (execvp(commands[0], commands) == -1)
                        {
                            exit(EXIT_FAILURE);
                        }
                        exit(EXIT_SUCCESS);
                    }
                    else
                    {
                        foreground_pid = pid;
                        add_process(foreground_pid, commands[0]);
                        int status;
                        waitpid(pid, &status, WUNTRACED);
                        if (WIFSTOPPED(status))
                        {
                            stop_process(pid);
                            foreground_pid = -1;
                        }
                        else
                        {
                            foreground_pid = -1;
                        }
                        remove_process(foreground_pid);
                        gettimeofday(&end, NULL);
                        int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                        if (time_taken >= 2)
                        {
                            strcpy(command, commands[0]);
                            *time = time_taken;
                        }
                        dup2(stdin_fd, STDIN_FILENO);
                        dup2(stdout_fd, STDOUT_FILENO);
                    }
                }
                exit(EXIT_SUCCESS);
            }
            else
            {
                int status;
                waitpid(pid_main, &status, 0);

                if (has_pipe)
                {
                    close(pipe_fds[pipe_index][1]);
                }
                dup2(stdin_fd, STDIN_FILENO);
                dup2(stdout_fd, STDOUT_FILENO);
                next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                pipe_index++;
            }
        }
        else
        {
            if (prev_has_pipe)
            {
                if (dup2(pipe_fds[pipe_index - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                    continue;
                }
                close(pipe_fds[pipe_index - 1][0]);
            }
            int redirect_output = 0;
            if (input_file)
            {
                if (open_input_file(input_filename))
                {
                    next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                    continue;
                }
            }
            if (output_file_write)
            {
                redirect_output = open_output_file(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else if (output_file_append)
            {
                redirect_output = open_output_file(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            if ((output_file_append || output_file_write) && !redirect_output)
            {
                dup2(stdin_fd, STDOUT_FILENO);
                next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                continue;
            }
            if (!has_pipe && set_stdout(redirect_output, output_file_append || output_file_write))
            {
                dup2(stdin_fd, STDOUT_FILENO);
                next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                continue;
            }

            if (background)
            {
                background_process(commands, new_count);
                dup2(stdin_fd, STDIN_FILENO);
                dup2(stdout_fd, STDOUT_FILENO);
                next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                continue;
            }
            if (!strcmp(commands[0], "hop"))
            {
                hop_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "reveal"))
            {
                reveal_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "log"))
            {
                log_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "proclore"))
            {
                proclore_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "seek"))
            {
                seek_main(home_dir, current_dir, old_dir, new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "activities"))
            {
                activities_main(new_count, command, time);
            }
            else if (!strcmp(commands[0], "ping"))
            {
                ping_main(new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "bg") || !strcmp(commands[0], "fg"))
            {
                command_main(new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "neonate"))
            {
                neonate_main(new_count, commands, command, time);
            }
            else if (!strcmp(commands[0], "iman"))
            {
                iman_main(commands, new_count, command, time);
            }
            else if (!strcmp(commands[0], "exit"))
            {
                for (int j = 0; j < MAX_LENGTH; j++)
                {
                    free(individual_commands[j]);
                    free(commands[j]);
                }
                dup2(stdin_fd, STDIN_FILENO);
                dup2(stdout_fd, STDOUT_FILENO);
                return 2;
            }
            else
            {
                struct timeval start, end;
                gettimeofday(&start, NULL);
                pid_t pid = fork();
                if (pid == 0)
                {
                    foreground_pid = getpid();
                    setpgid(0, 0);
                    commands[new_count] = NULL;
                    if (execvp(commands[0], commands) == -1)
                    {
                        next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                        close(stdin_fd);
                        close(stdout_fd);
                        continue;
                    }
                    next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
                    continue;
                }
                else
                {
                    foreground_pid = pid;
                    add_process(foreground_pid, commands[0]);
                    int status;
                    waitpid(pid, &status, WUNTRACED);
                    remove_process(foreground_pid);
                    if (WIFSTOPPED(status))
                    {
                        stop_process(pid);
                        foreground_pid = -1;
                    }
                    else
                    {
                        foreground_pid = -1;
                    }
                    gettimeofday(&end, NULL);
                    int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                    if (time_taken >= 2)
                    {
                        strcpy(command, commands[0]);
                        *time = time_taken;
                    }
                    dup2(stdin_fd, STDIN_FILENO);
                    dup2(stdout_fd, STDOUT_FILENO);
                }
            }
            dup2(stdin_fd, STDIN_FILENO);
            dup2(stdout_fd, STDOUT_FILENO);
            next_iteration(&i, counter_individual[index_individual], &index_individual, num_individual_commands, commands, individual_commands);
            pipe_index++;
        }
    }
    return 0;
}
