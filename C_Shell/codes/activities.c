#include "activities.h"

int compare_activities(const void *a, const void *b)
{
    return ((ProcessInfo *)a)->pid - ((ProcessInfo *)b)->pid;
}

void add_process(int pid_2, char *process_name)
{
    int index = 0;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (!processes[i].pid)
        {
            index = i;
            break;
        }
    }
    processes[index].pid = pid_2;
    strcpy(processes[index].cmd_name, process_name);
    strcpy(processes[index].state, "Running");
}

void remove_process(int pid_2)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid_2)
        {
            processes[i].pid = 0;
            break;
        }
    }
}

void stop_process(int pid_2)
{
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid == pid_2)
        {
            strcpy(processes[i].state, "Stopped");
            break;
        }
    }
}


int activities_main(int counter, char *command, int *time)
{
    if (counter > 1)
    {
        printf("Invalid Arguments\n");
        return 1;
    }
    struct timeval start, end;
    gettimeofday(&start, NULL);
    ProcessInfo proc_info[MAX_PROCESSES];
    int proc_index = 0;

    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if (processes[i].pid)
        {
            char path[256], state;
            snprintf(path, sizeof(path), "/proc/%d/status", processes[i].pid);
            FILE *file = fopen(path, "r");
            if (file == NULL)
            {
                processes[i].pid = 0;
                gettimeofday(&end, NULL);
                int time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
                if (time_taken >= 2)
                {
                    strcpy(command, "activities");
                    *time = time_taken;
                }
                continue;
            }
            char line[256];
            while (fgets(line, sizeof(line), file))
            {
                if (strncmp(line, "State:", 6) == 0)
                {
                    state = line[7];
                    break;
                }
            }
            fclose(file);
            if (state == 'T')
            {
                strcpy(processes[i].state, "Stopped");
            }
            if (state == 'Z')
            {
                processes[i].pid = 0;
                continue;
            }
            proc_info[proc_index] = processes[i];
            proc_index++;
        }
    }

    qsort(proc_info, proc_index, sizeof(ProcessInfo), compare_activities);

    for (int i = 0; i < proc_index; i++)
    {
        printf("%d : %s - %s\n", proc_info[i].pid, proc_info[i].cmd_name, proc_info[i].state);
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
