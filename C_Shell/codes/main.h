#ifndef _main_h_
#define _main_h_

#include "activities.h"
#include "helper.h"

extern ProcessInfo processes[MAX_PROCESSES];

extern int stdin_fd;
extern int stdout_fd;

extern char *alias[MAX_PROCESSES];
extern char *alias_function[MAX_PROCESSES];

#endif