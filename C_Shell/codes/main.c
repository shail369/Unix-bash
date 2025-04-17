#include "main.h"
#include "ping.h"

ProcessInfo processes[MAX_PROCESSES];

int stdout_fd;
int stdin_fd;

char *alias[MAX_PROCESSES];
char *alias_function[MAX_PROCESSES];

int main()
{
    char current_dir[MAX_LENGTH];
    char home_dir[MAX_LENGTH];
    char old_dir[MAX_LENGTH];
    old_dir[0] = '\0';
    char username[500];
    char systemname[500];
    char command[MAX_LENGTH];
    command[0] = '\0';
    int time = 0;
    get_details(username, systemname);
    getcwd(home_dir, sizeof(home_dir));
    getcwd(current_dir, sizeof(current_dir));
    get_prompt(current_dir, home_dir, username, systemname, command, time);
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].pid = 0;
    }
    int pid = getpid();
    processes[MAX_PROCESSES - 1].pid = pid;
    strcpy(processes[MAX_PROCESSES - 1].state, "Running");
    strcpy(processes[MAX_PROCESSES - 1].cmd_name, "Shell");
    stdin_fd = dup(STDIN_FILENO);
    stdout_fd = dup(STDOUT_FILENO);
    aliases();
    while (1)
    {
        signal(SIGINT, ctrl_c);
        signal(SIGTSTP, ctrl_z);
        command[0] = '\0';
        char original_command[MAX_LENGTH];
        original_command[0] = '\0';
        do
        {
            if (fgets(original_command, sizeof(original_command), stdin) == NULL)
            {
                if (feof(stdin))
                {
                    ctrl_d();
                    return 0;
                }
                else if (errno == EINTR)
                {
                    clearerr(stdin);
                    continue;
                }
                else
                {
                    perror("fgets");
                    return -1;
                }
            }
        } while (original_command == NULL);
        original_command[strcspn(original_command, "\n")] = '\0';
        if (solve(original_command, current_dir, home_dir, old_dir, command, &time) == 2)
        {
            break;
        }
        getcwd(current_dir, sizeof(current_dir));
        get_prompt(current_dir, home_dir, username, systemname, command, time);
    }
    return 0;
}
