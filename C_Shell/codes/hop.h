#ifndef _hop_h
#define _hop_

#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_LENGTH 4096

int hop(char *home_dir, char *current_dir, char *old_dir, char *directory, char *new_dir);

int hop_main(char *home_dir, char *current_dir, char *old_dir, int counter, char *commands[], char *command, int *time);

#endif