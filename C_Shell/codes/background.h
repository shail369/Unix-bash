#ifndef _background_h_
#define _background_h_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <signal.h>
#include "activities.h"
#include "helper.h"

#define MAX_LENGTH 4096

extern ProcessInfo processes[MAX_PROCESSES];
extern int stdin_fd;
extern int stdout_fd;

void handle_sigchld(int sig);

void background_process(char *commands[],int counter);

#endif