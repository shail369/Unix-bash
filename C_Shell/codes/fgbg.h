#ifndef _fgbg_h_
#define _fgbg_h_

#include "activities.h"
#include "helper.h"
#include <termios.h>
#include <errno.h>

extern ProcessInfo processes[MAX_PROCESSES];

int command_main(int counter, char *commands[], char *command, int *time);

int bg_command(pid_t pid);

int fg_command(pid_t pid, int shell_pgid);

#endif