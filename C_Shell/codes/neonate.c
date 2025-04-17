#include "neonate.h"

int stop = 0;

char get_keypress()
{
    struct termios oldt, newt;
    int oldf;
    char ch = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch;
}

pid_t find_pid()
{
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    pid_t most_recent_pid = -1;
    struct stat statbuf;
    time_t recent_time = 0;

    if (dir == NULL)
    {
        perror("Failed to open /proc directory");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (atoi(entry->d_name) > 0)
        {
            char proc_path[256];
            snprintf(proc_path, sizeof(proc_path), "/proc/%s", entry->d_name);

            if (stat(proc_path, &statbuf) == 0)
            {
                if (statbuf.st_ctime > recent_time)
                {
                    recent_time = statbuf.st_ctime;
                    most_recent_pid = atoi(entry->d_name);
                }
            }
        }
    }
    closedir(dir);
    return most_recent_pid;
}

void neonate(int time_arg)
{
    while (!stop)
    {
        if (get_keypress() == 'x')
        {
            stop = 1;
            break;
        }
        pid_t recent_pid = find_pid();
        if (recent_pid != -1)
        {
            printf("Most recent process PID: %d\n", recent_pid);
        }
        if (stop)
        {
            break;
        }
        sleep(time_arg);
    }
    printf("Stopped tracking process IDs.\n");
}

int neonate_main(int counter, char *commands[], char *command, int *time)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    if (counter != 3)
    {
        printf("Invalid Arguments\n");
    }
    int duration= atoi(commands[2]);
    neonate(duration);
    gettimeofday(&end, NULL);
    int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    if (time_taken >= 2)
    {
        strcpy(command, commands[0]);
        *time = time_taken;
    }
}