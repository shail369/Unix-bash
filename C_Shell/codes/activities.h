#ifndef _activities_h_
#define _activities_h_

#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/time.h>

typedef struct ProcessInfo
{
    char cmd_name[256];
    pid_t pid;
    char state[10];
} ProcessInfo;

#define MAX_PROCESSES 100

extern ProcessInfo processes[MAX_PROCESSES];

int compare_activities(const void *a, const void *b);

void add_process(int pid_2, char *process_name);

void remove_process(int pid_2);

void stop_process(int pid_2);

int activities_main();

#endif