#ifndef _ping_h_
#define _ping_h_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include "helper.h"

extern int foreground_pid;

int ping_main(int counter, char *commands[], char *command, int *time);

void ctrl_c(int sig);

void ctrl_d();

void ctrl_z(int sig);

#endif
