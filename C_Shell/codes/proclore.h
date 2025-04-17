#ifndef _proclore_h
#define _proclore_

#include <sched.h>
#include <stdbool.h>

int proclore(char *home_dir, pid_t pid);

int proclore_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time);

#endif